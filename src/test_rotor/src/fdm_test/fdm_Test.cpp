/****************************************************************************//*
 * Copyright (C) 2020 Marek M. Cel
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 ******************************************************************************/

#include <fdm_test/fdm_Test.h>

#include <cstring>
#include <iostream>

#include <fdm/fdm_Log.h>
#include <fdm/xml/fdm_XmlDoc.h>

////////////////////////////////////////////////////////////////////////////////

using namespace fdm;

////////////////////////////////////////////////////////////////////////////////

Test::Test( MainRotor::Direction direction ) :
    //_bladesCount ( 1 ),
    _bladesCount ( 4 ),
    _direction ( direction ),
    _blade ( FDM_NULLPTR )
{
    for ( int i = 0; i < _bladesCount; i++ )
    {
        Blade *blade = new Blade( _direction );

        if ( i == 0 )
        {
            _blade = blade;
        }

        _blades.push_back( blade );
    }

    for ( int i = 0; i < VECT_MAIN; i++ )
    {
        _blade->main[ i ].visible = false;
        strcpy( _blade->main[ i ].label, "" );
    }

    for ( int i = 0; i < VECT_SPAN; i++ )
    {
        _blade->span[ i ].visible = false;
        strcpy( _blade->span[ i ].label, "" );
    }

    readData( "../data/blade.xml" );

    for ( Blades::iterator it = _blades.begin(); it != _blades.end(); it++ )
    {
        (*it)->TEST_INIT();
    }

    _ned2enu(0,0) =  0.0;
    _ned2enu(0,1) =  1.0;
    _ned2enu(0,2) =  0.0;

    _ned2enu(1,0) =  1.0;
    _ned2enu(1,1) =  0.0;
    _ned2enu(1,2) =  0.0;

    _ned2enu(2,0) =  0.0;
    _ned2enu(2,1) =  0.0;
    _ned2enu(2,2) = -1.0;

    _enu2ned = _ned2enu.getTransposed();

    _bas2ned = Matrix3x3::createIdentityMatrix();
    _ned2bas = Matrix3x3::createIdentityMatrix();

    _bas2ras = Matrix3x3::createIdentityMatrix();
    _ras2bas = Matrix3x3::createIdentityMatrix();
}

////////////////////////////////////////////////////////////////////////////////

Test::~Test()
{
    FDM_DELPTR( _blade );

    for ( int i = 0; i < VECT_MAIN; i++ )
    {
        Data::get()->other.main[ i ].visible = false;

        Data::get()->other.main[ i ].b_x_enu = 0.0;
        Data::get()->other.main[ i ].b_y_enu = 0.0;
        Data::get()->other.main[ i ].b_z_enu = 0.0;

        Data::get()->other.main[ i ].v_x_enu = 0.0;
        Data::get()->other.main[ i ].v_y_enu = 0.0;
        Data::get()->other.main[ i ].v_z_enu = 0.0;

        strcpy( Data::get()->other.main[ i ].label, "" );
    }

    for ( int i = 0; i < VECT_SPAN; i++ )
    {
        Data::get()->other.span[ i ].visible = false;

        Data::get()->other.span[ i ].b_x_enu = 0.0;
        Data::get()->other.span[ i ].b_y_enu = 0.0;
        Data::get()->other.span[ i ].b_z_enu = 0.0;

        Data::get()->other.span[ i ].v_x_enu = 0.0;
        Data::get()->other.span[ i ].v_y_enu = 0.0;
        Data::get()->other.span[ i ].v_z_enu = 0.0;

        strcpy( Data::get()->other.span[ i ].label, "" );
    }

    for ( int i = 0; i < MAX_BLADES; i++ )
    {
        Data::get()->blade[ i ].beta  = 0.0;
        Data::get()->blade[ i ].theta = 0.0;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Test::readData( const std::string &dataFile )
{
    XmlDoc doc( dataFile );

    if ( doc.isOpen() )
    {
        XmlNode rootNode = doc.getRootNode();

        if ( rootNode.isValid() )
        {
            XmlNode nodeBlade = rootNode.getFirstChildElement( "blade" );

            for ( Blades::iterator it = _blades.begin(); it != _blades.end(); it++ )
            {
                (*it)->readData( nodeBlade );
            }
        }
        else
        {
            Log::e() << "Reading file \"" << dataFile << "\" failed. Invalid root node." << std::endl;
        }
    }
    else
    {
        Log::e() << "Reading file \"" << dataFile << "\" failed." << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Test::update( double timeStep )
{
    _ned2bas = Matrix3x3( Angles( Data::get()->state.ned_phi,
                                  Data::get()->state.ned_tht,
                                  Data::get()->state.ned_psi ) );
    _bas2ned = _ned2bas.getTransposed();

    Vector3 vel_wind_ned = Matrix3x3( Angles( 0.0, 0.0, -Data::get()->state.wind_dir ) )
             * Vector3( -Data::get()->state.wind_vel, 0.0, 0.0 );
    Vector3 vel_wind_bas = _ned2bas * vel_wind_ned;

    Vector3 vel_ras = Vector3( Data::get()->state.bas_u,
                               Data::get()->state.bas_v,
                               Data::get()->state.bas_w );

    Vector3 omg_ras = Vector3( Data::get()->state.bas_p,
                               Data::get()->state.bas_q,
                               Data::get()->state.bas_r );

    Vector3 vel_air_ras = vel_ras - vel_wind_bas;
    Vector3 omg_air_ras = omg_ras;

    Vector3 grav_ned( 0.0, 0.0, WGS84::_g );
    Vector3 grav_bas = _ned2bas * grav_ned;
    Vector3 grav_ras = _bas2ras * grav_bas;

    //std::cout << grav_ras.toString() << std::endl;

    double omega   = Data::get()->rotor.omega;
    double azimuth = Data::get()->rotor.azimuth;
    double airDensity = 1.225;

    double delta_psi = 2.0 * M_PI / (double)(_bladesCount);

    double theta_0  = Data::get()->rotor.collective;
    double theta_1c = ( _direction == MainRotor::CCW ? -1.0 : 1.0 ) * Data::get()->rotor.cyclicLat;
    double theta_1s = Data::get()->rotor.cyclicLon;

    int index = 0;

    for ( Blades::iterator it = _blades.begin(); it != _blades.end(); it++ )
    {
        (*it)->update( timeStep,
                       vel_air_ras,
                       omg_air_ras,
                       omg_ras,
                       grav_ras,
                       omega,
                       azimuth + index * delta_psi,
                       airDensity,
                       theta_0,
                       theta_1c,
                       theta_1s
                       );

        index++;
    }

    //////////////

    for ( int i = 0; i < VECT_MAIN; i++ )
    {
        updateDataVect( Data::get()->other.main[ i ], _blade->main[ i ], azimuth );
    }

    for ( int i = 0; i < VECT_SPAN; i++ )
    {
        updateDataVect( Data::get()->other.span[ i ], _blade->span[ i ], azimuth );
    }

    if ( 0 )
    {
        Vector3 v_ras =  vel_wind_bas;
        Vector3 v_bas = _ras2bas * v_ras;
        Vector3 v_ned = _bas2ned * v_bas;
        Vector3 v_enu = _ned2enu * v_ned;

        Data::get()->other.main[ 0 ].visible = true;

        Data::get()->other.main[ 0 ].b_x_enu = 0.0;
        Data::get()->other.main[ 0 ].b_y_enu = 0.0;
        Data::get()->other.main[ 0 ].b_z_enu = 0.0;

        Data::get()->other.main[ 0 ].v_x_enu = v_enu.x();
        Data::get()->other.main[ 0 ].v_y_enu = v_enu.y();
        Data::get()->other.main[ 0 ].v_z_enu = v_enu.z();

        strcpy( Data::get()->other.main[ 0 ].label, "WIND" );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Test::updateData()
{
    int index = 0;

    for ( Blades::iterator it = _blades.begin(); it != _blades.end(); it++ )
    {
        Data::get()->blade[ index ].beta  = (*it)->getBeta();
        Data::get()->blade[ index ].theta = (*it)->getTheta();

        index++;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Test::updateDataVect( Data::Other::Vector &vect,
                           const Blade::Vect &out,
                           double azimuth )
{
    MainRotor::Direction direction = MainRotor::CW;

    if ( Data::get()->rotor.direction == Data::Rotor::CCW )
    {
        direction = MainRotor::CCW;
    }

    Matrix3x3 ras2sra = _blade->getRAS2SRA( azimuth, direction );
    Matrix3x3 sra2ras = ras2sra.getTransposed();

    Vector3 b_ras =  sra2ras * out.b_sra;
    Vector3 b_bas = _ras2bas * b_ras;
    Vector3 b_ned = _bas2ned * b_bas;
    Vector3 b_enu = _ned2enu * b_ned;

    Vector3 v_ras =  sra2ras * out.v_sra;
    Vector3 v_bas = _ras2bas * v_ras;
    Vector3 v_ned = _bas2ned * v_bas;
    Vector3 v_enu = _ned2enu * v_ned;

    vect.visible = out.visible;

    vect.b_x_enu = b_enu.x();
    vect.b_y_enu = b_enu.y();
    vect.b_z_enu = b_enu.z();

    vect.v_x_enu = v_enu.x();
    vect.v_y_enu = v_enu.y();
    vect.v_z_enu = v_enu.z();

    strcpy( vect.label, out.label );
}

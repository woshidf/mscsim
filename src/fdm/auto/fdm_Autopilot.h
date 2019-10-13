/****************************************************************************//*
 * Copyright (C) 2019 Marek M. Cel
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
#ifndef FDM_AUTOPILOT_H
#define FDM_AUTOPILOT_H

////////////////////////////////////////////////////////////////////////////////

#include <fdm/fdm_Defines.h>

#include <fdm/sys/fdm_PID.h>
#include <fdm/utils/fdm_Table.h>
#include <fdm/xml/fdm_XmlNode.h>

#include <fdm/auto/fdm_FlightDirector.h>

////////////////////////////////////////////////////////////////////////////////

namespace fdm
{

/**
 * @brief Autopilot class.
 *
 * @see McLean D. - Automatic Flight Control Systems, 1990
 * @see Roskam J. - Airplane Flight Dynamics and Automatic Flight Controls, 2001
 */
class FDMEXPORT Autopilot
{
public:

    /** Constructor. */
    Autopilot();

    /** Destructor. */
    virtual ~Autopilot();

    /** Initializes autopilot. */
    virtual void init();

    /**
     * Reads data.
     * @param dataFile XML data file path
     */
    virtual void readData( const std::string &dataFile );

    /**
     * @brief Updates autopilot.
     * @param timeStep [s]
     * @param roll [rad]
     * @param pitch [rad]
     * @param heading [rad]
     * @param airspeed [m/s]
     * @param altitude [m]
     * @param climbRate [m/s]
     * @param yawRate [rad/s]
     * @param turnRate [rad/s]
     * @param deviation_hor [-1.0;1.0]
     * @param deviation_ver [-1.0;1.0]
     * @param buttonDN
     * @param buttonUP
     */
    virtual void update( double timeStep,
                         double roll, double pitch, double heading,
                         double altitude, double airspeed,
                         double climbRate, double yawRate, double turnRate,
                         double deviation_hor, double deviation_ver,
                         bool buttonDN, bool buttonUP );

    inline double getMinAltitude()  const { return _min_alt;  }
    inline double getMaxAltitude()  const { return _max_alt;  }
    inline double getMinClimbRate() const { return _min_vs; }
    inline double getMaxClimbRate() const { return _max_vs; }

    inline double getAltitude()  const { return _fd->getAltitude();  }
    inline double getAirspeed()  const { return _fd->getAirspeed();  }
    inline double getHeading()   const { return _fd->getHeading();   }
    inline double getClimbRate() const { return _fd->getClimbRate(); }
    inline double getPitch()     const { return _fd->getPitch();     }

    inline double getCmdRoll()  const { return _fd->getCmdRoll();  }
    inline double getCmdPitch() const { return _fd->getCmdPitch(); }

    inline double getCtrlRoll()  const { return _ctrl_roll;  }
    inline double getCtrlPitch() const { return _ctrl_pitch; }
    inline double getCtrlYaw()   const { return _ctrl_yaw;   }

    inline bool isActiveAP() const { return _engaged; }
    inline bool isActiveFD() const { return _fd->isEngaged(); }
    inline bool isActiveYD() const { return _yawDamper; }

    inline bool isActiveALT() const { return _fd->getVerMode() == FlightDirector::VM_ALT; }
    inline bool isActiveIAS() const { return _fd->getVerMode() == FlightDirector::VM_IAS; }
    inline bool isActiveVS()  const { return _fd->getVerMode() == FlightDirector::VM_VS;  }
    inline bool isActiveARM() const { return _fd->getVerMode() == FlightDirector::VM_ARM; }
    inline bool isActiveGS()  const { return _fd->getVerMode() == FlightDirector::VM_GS;  }
    inline bool isActiveHDG() const { return _fd->getHorMode() == FlightDirector::HM_HDG; }
    inline bool isActiveNAV() const { return _fd->getHorMode() == FlightDirector::HM_NAV; }
    inline bool isActiveAPR() const { return _fd->getHorMode() == FlightDirector::HM_APR; }
    inline bool isActiveBC()  const { return _fd->getHorMode() == FlightDirector::HM_BC;  }

    inline bool isActiveSoftRide() const { return _softRide; }
    inline bool isActiveHalfBank() const { return _halfBank; }

    void setAltitude( double altitude );
    void setAirspeed( double airspeed );
    void setClimbRate( double climbRate );
    void setHeading( double heading );
    void setPitch( double pitch );

protected:

    FlightDirector *_fd;    ///< flight director

    PID _pid_r;             ///< roll PID controller
    PID _pid_p;             ///< pitch PID controller
    PID _pid_y;             ///< yaw PID controller

    Table _gain_ias_r;      ///< roll gain schedule due to airspeed
    Table _gain_ias_p;      ///< pitch gain schedule due to airspeed
    Table _gain_ias_y;      ///< yaw gain schedule due to airspeed

    double _max_rate_roll;  ///< [1/s] max control roll rate
    double _max_rate_pitch; ///< [1/s] max control pitch rate
    double _max_rate_yaw;   ///< [1/s] max control yaw rate

    double _min_pitch;      ///< [rad] min pitch
    double _max_pitch;      ///< [rad] max pitch
    double _min_alt;        ///< [m]   min altitude
    double _max_alt;        ///< [m]   max altitude
    double _min_ias;        ///< [m/s] min airspeed
    double _max_ias;        ///< [m/s] max airspeed
    double _min_vs;         ///< [m/s] min climb rate
    double _max_vs;         ///< [m/s] max climb rate

    double _rate_pitch;     ///< [deg/s] pitch rate of change
    double _rate_alt;       ///< [m/s] altitude rate of change
    double _rate_ias;       ///< [(m/s)/s] airspeed rate of change
    double _rate_vs;        ///< [(m/s)/s] vertical speed rate of change

    double _softRideCoef;   ///< [-] soft ride coefficient

    double _ctrl_roll;      ///< roll control command
    double _ctrl_pitch;     ///< pitch control command
    double _ctrl_yaw;       ///< yaw control command

    bool _yawDamper;        ///< specify if yaw damper is engaged

    bool _softRide;         ///< specify if soft ride is engaged
    bool _halfBank;         ///< specify if half bank is engaged

    bool _testing;          ///< specify if test is active
    bool _engaged;          ///< specify if autopilot is engaged

    virtual void readChannel( const fdm::XmlNode &dataNode, double &max_rate,
                              PID &pid, Table &gain_ias );

    virtual void readPID( const fdm::XmlNode &dataNode, PID &pid );
};

} // end of fdm namespace

////////////////////////////////////////////////////////////////////////////////

#endif // FDM_AUTOPILOT_H
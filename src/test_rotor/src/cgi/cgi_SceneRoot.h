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
#ifndef CGI_SCENEROOT_H
#define CGI_SCENEROOT_H

////////////////////////////////////////////////////////////////////////////////

#include <osg/Group>

#include <cgi/cgi_Helicopter.h>
#include <cgi/cgi_Terrain.h>
#include <cgi/cgi_Vector.h>
#include <cgi/cgi_Windsock.h>

////////////////////////////////////////////////////////////////////////////////

namespace cgi
{

/** */
class SceneRoot
{
public:

    /** */
    SceneRoot();

    /** */
    virtual ~SceneRoot();

    void update();

    /** Returns root node. */
    inline osg::ref_ptr<osg::Group> getRoot() { return _root; }

private:

    osg::ref_ptr<osg::Group> _root;

    Helicopter *_helicopter;
    Terrain    *_terrain;
    Vector     *_vector;
    Windsock   *_windsock;

    void createSceneLight();
};

} // end of cgi namespace

////////////////////////////////////////////////////////////////////////////////

#endif // CGI_SCENEROOT_H

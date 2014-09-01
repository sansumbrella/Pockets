/*
 * Copyright (c) 2013, David Wicks
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "Pockets.h"
#include "cinder/gl/Vbo.h"

namespace pockets
{
  /**
  An expanded line that attempts to orient the bulk of its width toward the viewer.
  */
  typedef std::shared_ptr<class ExpandedPath3d> ExpandedPath3dRef;
  typedef std::unique_ptr<class ExpandedPath3d> ExpandedPath3dUniqueRef;
  class ExpandedPath3d
  {
  public:
    ExpandedPath3d( size_t length );
    ~ExpandedPath3d();
    //! accepts a float in range [0,1] and returns a float (probably in range [0,1])
    typedef std::function<float (float)> ShapeFn;
    //! set the shape function for the width of the line along its length
    void setShapeFn( ShapeFn fn ){ mShapeFn = fn; mDirty = true; }
    void setPositions( const std::vector<ci::vec3> &positions, const ci::vec3 &eye_axis=ci::vec3::zAxis() );
    void setWidth( float w ){ mLineHalfWidth = w / 2; mDirty = true; }
    void draw();
    //! create and expanded path with \a length spine vertices
    static ExpandedPath3dUniqueRef create( size_t length );
  private:
    void buildOutline();
    float getHalfWidth( float t );
    ShapeFn                 mShapeFn = [](float f){ return 1.0f; };
    bool                    mDirty = true;
    //! skeleton of vertices
    std::vector<ci::vec3>  mSkeleton;
    //! expanded vertices
    std::vector<ci::vec3>  mOutline;
    float                   mLineHalfWidth = 4.0f;
    ci::vec3               mEyeAxis;
    ci::gl::VboMesh         mVbo;
  };

}

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

namespace pockets
{
  /**
  An expanded line in two dimensions.
  */
  typedef std::shared_ptr<class FatLine2d> FatLine2dRef;
  class FatLine2d
  {
  public:
    FatLine2d( size_t vertex_count );
    ~FatLine2d();
    //! accepts a float in range [0,1] and returns a float (probably in range [0,1])
    typedef std::function<float (float)> ShapeFn;
    //! set the shape function for the width of the line along its length
    void setShapeFn( ShapeFn fn ){ mShapeFn = fn; }
    void setPositions( const std::vector<ci::Vec2f> &positions, bool closed=false );
    void setWidth( float w ){ mLineHalfWidth = w / 2; }
    void draw();
    static FatLine2dRef create( size_t vertex_count ){
      return FatLine2dRef( new FatLine2d( vertex_count ) );
    }
  private:
    void buildOutline();
    float getHalfWidth( float t );
    ShapeFn                 mShapeFn = [](float f){ return 1.0f; };
    //! skeleton of vertices
    std::vector<ci::Vec2f>  mSkeleton;
    //! expanded vertices
    std::vector<ci::Vec2f>  mOutline;
    std::vector<ci::Vec2f>  mTexCoords;
    float                   mLineHalfWidth = 4.0f;
    bool                    mClosed = false;
    // ci::gl::VboMesh         mVbo;
  };

}

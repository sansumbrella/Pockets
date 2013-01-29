//
//  FatLine3d.h
//  Energy
//
//  Created by David Wicks on 1/25/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "Pockets.h"
#include "cinder/gl/Vbo.h"

namespace pockets
{
  /**
  An expanded line that attempts to orient the bulk of its width toward the viewer.
  */
  class FatLine3d
  {
  public:
    FatLine3d( size_t length );
    ~FatLine3d();
    //! accepts a float in range [0,1] and returns a float (probably in range [0,1])
    typedef std::function<float (float)> ShapeFn;
    //! set the shape function for the width of the line along its length
    void setShapeFn( ShapeFn fn ){ mShapeFn = fn; }
    void setPositions( const std::vector<ci::Vec3f> &positions, const ci::Vec3f &eye_axis=ci::Vec3f::zAxis() );
    void setWidth( float w ){ mLineHalfWidth = w / 2; }
    void draw();
  private:
    void buildOutline( const ci::Vec3f &eye_axis );
    float getHalfWidth( float t );
    ShapeFn                 mShapeFn = [](float f){ return 1.0f; };
    //! skeleton of vertices
    std::vector<ci::Vec3f>  mSkeleton;
    //! expanded vertices
    std::vector<ci::Vec3f>  mOutline;
    float                   mLineHalfWidth = 4.0f;
    ci::gl::VboMesh         mVbo;
  };
  
}
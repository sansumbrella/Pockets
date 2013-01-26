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

  class FatLine3d
  {
  public:
    FatLine3d( size_t length );
    ~FatLine3d();
    void setPositions( const std::vector<ci::Vec3f> &positions, const ci::Vec3f &eye_axis=ci::Vec3f::zAxis() );
    void setWidth( float w ){ mLineWidth = w; }
    void draw();
  private:
    void buildOutline( const ci::Vec3f &eye_axis );
    //! skeleton of vertices
    std::vector<ci::Vec3f>  mSkeleton;
    std::vector<ci::Vec3f>  mOutline;
    float                   mLineWidth = 4.0f;
    ci::gl::VboMesh         mVbo;
  };
  
}
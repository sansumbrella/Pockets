//
//  AnimationUtils.h
//  Energy
//
//  Created by David Wicks on 1/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "Pockets.h"

namespace pockets
{

  //! quaternion lerp function suitable for passing to ci::Timeline
  ci::Quatf lerpQuaternion( const ci::Quatf &start, const ci::Quatf &end, float time );
  //! lerp between two values over the shortest path that wraps around at 0 and 1
  float lerpShortestNormalized( float start, float end, float time );
  //! lerp color through HSV space; wraps Hue from 0—1
  ci::ColorA lerpHSVA( const ci::ColorA &start, const ci::ColorA &finish, float time );
  //! are all components of a vector finite?
  inline bool isfinite( const ci::Vec3f &vec );

  class WrappedLerper
  {
  public:
    WrappedLerper( float aStart, float aEnd );
    float getPosition( float t );
    //! how far from begin to end around the circle?
    float length() const { return ci::math<float>::abs( mEnd - mStart ); }
  private:
    float mStart;
    float mEnd;
  };
}
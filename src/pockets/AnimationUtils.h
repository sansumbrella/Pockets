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
  //! returns f with precision of 1/steps (e.g. for 0.31 with 5 steps, yields 0.4, with 10 steps, yields 0.3 )
	float quantize( float f, float steps );

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
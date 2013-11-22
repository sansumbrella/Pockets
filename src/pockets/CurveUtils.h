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

#include "Pockets.h"
#include <vector>
#include "cinder/Vector.h"
#include "cinder/BSpline.h"

#pragma once

namespace pockets
{

/**
returns a vector of points and control points suitable for constructing
a ci::BSpline. Accepts a collection of either Vec2fs or Vec3fs.
e.g. BSpline3f spline( curveThrough( points ), 3, false, true );
*/
template <typename T>
std::vector<T> curveThrough( const std::vector<T> &points );

/**
Cache for arc-length to time conversion.
Reparameterizes a spline along the normalized arc-length.
Enables constant-speed travel along curves at interactive rates.
TODO: rename
*/
class SplineArcLengthParameterizer
{
public:
  //! Get the curve time from normalized arc-length s[0,1] on sampled curve
  float getTime( float s ) const;
  //! Get the curve position from normalized arc-length s[0,1] on sampled curve
  ci::Vec3f getPosition( float s ) const;
  //! Calculates t/s relationship for \a spline and stores spline for position lookup.
  void sampleCurve( const ci::BSpline3f &spline, const int numSamples = 64 );
private:
  // a Sample stores the arc position, time, and slope of the curve at a given point
  struct Sample
  {
    float t = 0.0f;
    float s = 0.0f;
    // slope dt/ds between previous sample and this one
    float slope = 0.0f;
  };
  std::vector<Sample> mSamples;
  float               mArcLength = 0.0f;
  ci::BSpline3f       mSpline;
};

} // pockets

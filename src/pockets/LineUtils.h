

#include "Pockets.h"
#include <vector>

#pragma once

namespace pockets
{

//! returns a vector of points and control points suitable for constructing
//! a ci::BSpline. Accepts a collection of either Vec2fs or Vec3fs.
//! e.g. BSpline3f spline( curveThrough( points ), 3, false, true );
template <typename T>
std::vector<T> curveThrough( const std::vector<T> &points );

/**
Cache for arc-length to time conversion
Enables constant-speed travel along curve at interactive rates
*/
class SplineArcLengthParameterizer
{
  struct Sample
  {
    float t = 0.0f;
    float s = 0.0f;
    // slope from previous sample up to this one
    float slope = 0.0f;
  };
public:
  // Get the curve time from normalized arc-length s
  float getTime( float s ) const;
  // Get the curve position from normalized arc-length s
  ci::Vec3f getPosition( float s ) const;
  void sampleCurve( const ci::BSpline3f &spline, const int numSamples = 64 );
private:
  std::vector<Sample> mSamples;
  float         mArcLength = 1.0f;
  ci::BSpline3f mSpline;
};

} // pockets

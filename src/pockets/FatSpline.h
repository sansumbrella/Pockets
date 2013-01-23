//
//  FatSpline.h
//  Energy
//
//  Created by David Wicks on 1/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "Pockets.h"
#include "cinder/gl/Vbo.h"

namespace pockets
{

typedef std::shared_ptr<class FatSpline> FatSplineRef;
class FatSpline
{
public:
  FatSpline( const std::vector<ci::Vec3f> &through_points, int subdivisions=4, bool closed=false );
  ~FatSpline();

  //! access base positions
  const std::vector<ci::Vec3f>& basePositions() const { return mBasePositions; }
  //! access current positions
  const std::vector<ci::Vec3f>& positions() const { return mSkeletonPositions; }
  //! access (and mutate) offsets from base
  std::vector<ci::Vec3f>& positionOffsets(){ return mSkeletonOffsets; }

  void setWidth( float w ){ mLineWidth = w; }
  float getLineWidth() const { return mLineWidth; }
  void setColor( const ci::ColorA &color ){ mColor = color; }
  ci::Matrix44f& skeletonTransform(){ return mSkeletonTransform; }

  void update();
  void draw();

  //! calls \a fn once for each position using the return value as a scalar
  //! multiplier for an offset along the (2D clockwise) normal at that position
  void applyNormalOffset( std::function<float (float)> fn );
  //! calls \a fn once for each position, adding the returned value as an
  //! offset to each position
  void applyOffset(std::function<ci::Vec3f (float)> fn);
  //! returns a new spline that curves through the provided through_points
  static std::shared_ptr<FatSpline> create( const std::vector<ci::Vec3f> &through_points, int subdivisions=4, bool closed=false )
  { return std::shared_ptr<FatSpline>( new FatSpline( through_points, subdivisions, closed ) ); }
private:
  ci::Matrix44f	mSkeletonTransform = ci::Matrix44f::identity();
  ci::ColorA		mColor = ci::ColorA::white();
  bool			mIsClosed = false;
  //! our untransformed center line
  std::vector<ci::Vec3f>	mBasePositions;
  //! our center line offsets
  std::vector<ci::Vec3f>	mSkeletonOffsets;
  //! our transformed center line
  std::vector<ci::Vec3f>	mSkeletonPositions;
  //! positions derived from a BSpline3f through the mSkeletonPositions
  std::vector<ci::Vec3f>	mDerivedPositions;
  //! positions on the edge of the line, so we can draw as geometry
  std::vector<ci::Vec3f>	mOutlinePositions;
  ci::gl::VboMesh		mVbo;
  float             mLineWidth;
};
}
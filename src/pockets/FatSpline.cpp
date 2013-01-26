//
//  FatSpline.cpp
//  Energy
//
//  Created by David Wicks on 1/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "FatSpline.h"
#include "pockets/LineUtils.h"

using namespace pockets;
using namespace std;
using namespace ci;

FatSpline::FatSpline( const vector<Vec3f> &through_points, int subdivisions, bool closed ):
mBasePositions( through_points ),
mSkeletonPositions( through_points ),
mIsClosed( closed )
{
  mSkeletonOffsets.assign( mBasePositions.size(), Vec3f::zero() );
  mDerivedPositions.assign( mBasePositions.size() * subdivisions, Vec3f::zero() );
  mOutlinePositions.assign( mDerivedPositions.size() * 2, Vec3f::zero() );

  vector<ColorA> colors;
  colors.assign( mOutlinePositions.size(), ColorA::white() );

  vector<Vec2f> texcoords;
  for ( int i = 0; i != mDerivedPositions.size(); ++i )
  {
    float factor = i / (mDerivedPositions.size() - 1.0f);
    texcoords.push_back( Vec2f( factor, 0 ) ); // north
    texcoords.push_back( Vec2f( factor, 1 ) ); // south
  }

  if( !mVbo )
  {
    gl::VboMesh::Layout layout;
    layout.setDynamicPositions();
    layout.setStaticTexCoords2d();
    mVbo = gl::VboMesh( mOutlinePositions.size(), 0, layout, GL_TRIANGLE_STRIP );
    mVbo.bufferTexCoords2d( 0, texcoords );
    auto iter = mVbo.mapVertexBuffer();
    for( const auto &loc : mOutlinePositions )
    {
      iter.setPosition( loc );
      ++iter;
    }
  }
  update();
}

FatSpline::~FatSpline()
{}

void FatSpline::setPositions( const std::vector<ci::Vec3f> &through_points )
{
  assert( through_points.size() == mBasePositions.size() );
  mBasePositions = through_points;
  update();
}

void FatSpline::update()
{
  // apply offsets
  for( int i = 0; i != mSkeletonPositions.size(); ++i )
  {
    mSkeletonPositions.at(i) = mBasePositions.at(i) + mSkeletonOffsets.at(i);
  }

  // derive spline from skeleton
  BSpline3f spline( curveThrough( mSkeletonPositions ), 3, mIsClosed, !mIsClosed );
  int count = mDerivedPositions.size();
  for( int i = 0; i != count; ++i )
  {
    mDerivedPositions[i] = spline.getPosition( i / (count - 1.0f) );
  }

  Vec3f a, b, c;
  // first vertex
  a = mDerivedPositions.at( 0 );
  b = mDerivedPositions.at( 1 );
  Vec3f edge = (b - a).normalized() * mLineWidth;
  Vec3f north( -edge.y, edge.x, 0.0f );
  Vec3f south( -north );
  mOutlinePositions.at( 0 ) = a + north;
  mOutlinePositions.at( 0 + 1) = a + south;
  // intermediate vertices
  for( int i = 1; i < mDerivedPositions.size() - 1; ++i )
  {
    a = mDerivedPositions.at(i - 1);
    b = mDerivedPositions.at(i);
    c = mDerivedPositions.at(i + 1);
    edge = (b - a).normalized();
    Vec3f nextEdge = (c - b).normalized();
    Vec3f avg = ((edge + nextEdge) * 0.5f).normalized() * mLineWidth;
    // if edges point in opposite directions
    //		if( edge.dot( nextEdge ) < 0.0f )
    //		{	// rotate average 90 degrees to avoid sharp point
    //			avg = Vec3f( -avg.y, avg.x, 0.0f );
    //		}
    north = Vec3f( -avg.y, avg.x, 0.0f );
    south = Vec3f( -north );
    mOutlinePositions.at( i * 2 ) = b + north;
    mOutlinePositions.at( i * 2 + 1 ) = b + south;
  }
  // last vertex
  c = mDerivedPositions.back();
  Vec3f segment = mIsClosed ? (mDerivedPositions.at(1) - c) : (c - b);
  edge = segment.normalized() * mLineWidth;

  north = Vec3f( -edge.y, edge.x, 0.0f );
  south = -north;
  int end = mDerivedPositions.size() - 1;
  mOutlinePositions.at( end * 2 ) = c + north;
  mOutlinePositions.at( end * 2 + 1) = c + south;

  auto iter = mVbo.mapVertexBuffer();
  for( const auto &loc : mOutlinePositions )
  {
    iter.setPosition( loc );
    ++iter;
  }
}

void FatSpline::draw()
{
  gl::draw( mVbo );
}

void FatSpline::applyNormalOffset( std::function<float (float)> fn )
{
  float end = mSkeletonPositions.size();
  for( int i = 1; i < end - 1; ++i )
  {
    float t = i / (end - 1.0f);
    Vec3f a = mSkeletonPositions.at( i - 1 );
    Vec3f b = mSkeletonPositions.at( i );
    Vec3f c = mSkeletonPositions.at( i + 1 );
    Vec2f edge( b.y - a.y, a.x - b.x );
    Vec2f nextEdge( c.y - b.y, b.x - c.x );
    Vec2f avg((edge + nextEdge) * 0.5f);
    avg.safeNormalize();
    Vec3f normal( avg.x, avg.y, 0.0f );
    mSkeletonOffsets.at(i) += fn( t ) * normal;
  }
  if( mIsClosed )
  {
    size_t end = mSkeletonPositions.size();
    Vec3f a = mSkeletonPositions.at( end - 1 );
    Vec3f b = mSkeletonPositions.at( 0 );
    Vec3f c = mSkeletonPositions.at( 1 );
    Vec2f edge( b.y - a.y, a.x - b.x );
    Vec2f nextEdge( c.y - b.y, b.x - c.x );
    Vec2f avg((edge + nextEdge) * 0.5f);
    avg.safeNormalize();
    Vec3f normal( avg.x, avg.y, 0.0f );
    mSkeletonOffsets.at(0) += fn( 0.0f ) * normal;
    mSkeletonOffsets.at(end - 1) += fn( 1.0f ) * normal;
  }
  else
  {
    size_t end = mSkeletonPositions.size();
    Vec3f a = mSkeletonPositions.at( end - 2 );
    Vec3f b = mSkeletonPositions.at( end - 1 );
    Vec2f edge( b.y - a.y, a.x - b.x );
    edge.safeNormalize();
    Vec3f normal( edge.x, edge.y, 0.0f );
    mSkeletonOffsets.at( 0 ) += fn( 0 ) * normal;

    a = mSkeletonPositions.at( 0 );
    b = mSkeletonPositions.at( 1 );
    edge.set( b.y - a.y, a.x - b.x );
    edge.safeNormalize();
    normal = Vec3f( edge.x, edge.y, 0.0f );
    mSkeletonOffsets.at( 0 ) += fn( 1.0f ) * normal;
    
  }
}

void FatSpline::applyOffset(std::function<ci::Vec3f (float)> fn)
{
  int end = mSkeletonOffsets.size();
  for( int i = 0; i < end; ++i )
  {
    float t = i / (end - 1.0f);
    mSkeletonOffsets.at(i) += fn( t );
  }
}


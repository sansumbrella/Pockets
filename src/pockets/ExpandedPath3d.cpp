//
//  ExpandedPath3d.cpp
//  Energy
//
//  Created by David Wicks on 1/25/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "ExpandedPath3d.h"
#include "cinder/CinderMath.h"

using namespace pockets;
using namespace std;
using namespace ci;

ExpandedPath3d::ExpandedPath3d( size_t length )
{
  mSkeleton.assign( length, Vec3f::zero() );
  mOutline.assign( length * 2, Vec3f::zero() );

  vector<Vec2f> texcoords;
//  vector<Color> colors;
  for ( int i = 0; i != length; ++i )
  {
    float factor = i / (length - 1.0f);
    texcoords.push_back( Vec2f( factor, 0 ) ); // north
    texcoords.push_back( Vec2f( factor, 1 ) ); // south
//    colors.push_back( Color( CM_HSV, lmap<float>( i, 0, length-1, 0.0f, 1.0f ), 1.0f, 1.0f ) );
//    colors.push_back( Color( CM_HSV, 0.0f, 0.0f, 1.0f ) );
  }

  if( !mVbo )
  {
    gl::VboMesh::Layout layout;
    layout.setDynamicPositions();
    layout.setStaticTexCoords2d();
//    layout.setStaticColorsRGB();
    mVbo = gl::VboMesh( length * 2, 0, layout, GL_TRIANGLE_STRIP );
    mVbo.bufferTexCoords2d( 0, texcoords );
//    mVbo.bufferColorsRGB( colors );
    auto iter = mVbo.mapVertexBuffer();
    for( const auto &loc : mOutline )
    {
      iter.setPosition( loc );
      ++iter;
    }
  }
}

ExpandedPath3d::~ExpandedPath3d()
{}

ExpandedPath3dUniqueRef ExpandedPath3d::create( size_t length )
{
  return ExpandedPath3dUniqueRef{ new ExpandedPath3d{ length } };
}

void ExpandedPath3d::setPositions( const vector<Vec3f> &positions, const Vec3f &eye_axis )
{
  assert( positions.size() == mSkeleton.size() );
  mSkeleton = move( positions );
  buildOutline( eye_axis );
}

void ExpandedPath3d::buildOutline( const Vec3f &eye_axis )
{
  Vec3f a, b;
  const float last_index = mSkeleton.size() - 1.0f;
  // first vertex
  a = mSkeleton.at( 0 );
  b = mSkeleton.at( 1 );
  Vec3f edge = (b - a).normalized();
  Vec3f tangent = edge.cross( eye_axis );
  Vec3f north = tangent.normalized() * getHalfWidth( 0 );
  Vec3f south = -north;
  mOutline.at( 0 ) = a + north;
  mOutline.at( 0 + 1) = a + south;
  // remaining vertices
  for( int i = 1; i < mSkeleton.size(); ++i )
  {
    a = mSkeleton.at(i - 1);
    b = mSkeleton.at(i);
    edge = (b - a).normalized();
    tangent = edge.cross( eye_axis );
    north = tangent.normalized() * getHalfWidth( i / last_index );
    south = -north;
    mOutline.at( i * 2 ) = b + north;
    mOutline.at( i * 2 + 1 ) = b + south;
  }

  auto iter = mVbo.mapVertexBuffer();
  for( const auto &loc : mOutline )
  {
    iter.setPosition( loc );
    ++iter;
  }
}

float ExpandedPath3d::getHalfWidth( float t )
{
  return mShapeFn( t ) * mLineHalfWidth;
}

void ExpandedPath3d::draw()
{
  gl::draw( mVbo );
}

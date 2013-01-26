//
//  FatLine3d.cpp
//  Energy
//
//  Created by David Wicks on 1/25/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "FatLine3d.h"
#include "cinder/CinderMath.h"

using namespace pockets;
using namespace std;
using namespace ci;

FatLine3d::FatLine3d( size_t length )
{
  mSkeleton.assign( length, Vec3f::zero() );
  mOutline.assign( length * 2, Vec3f::zero() );

  vector<Vec2f> texcoords;
  vector<Color> colors;
  for ( int i = 0; i != length; ++i )
  {
    float factor = i / (length - 1.0f);
    texcoords.push_back( Vec2f( factor, 0 ) ); // north
    texcoords.push_back( Vec2f( factor, 1 ) ); // south
    colors.push_back( Color( CM_HSV, lmap<float>( i, 0, length-1, 0.0f, 1.0f ), 1.0f, 1.0f ) );
    colors.push_back( Color( CM_HSV, 0.0f, 0.0f, 1.0f ) );
  }

  if( !mVbo )
  {
    gl::VboMesh::Layout layout;
    layout.setDynamicPositions();
    layout.setStaticTexCoords2d();
    layout.setStaticColorsRGB();
//    layout.setStaticIndices();
    mVbo = gl::VboMesh( length * 2, 0, layout, GL_QUAD_STRIP );
    mVbo.bufferTexCoords2d( 0, texcoords );
    mVbo.bufferColorsRGB( colors );
    auto iter = mVbo.mapVertexBuffer();
    for( const auto &loc : mOutline )
    {
      iter.setPosition( loc );
      ++iter;
    }
  }
  buildOutline();
}

FatLine3d::~FatLine3d()
{}

void FatLine3d::setPositions( const vector<Vec3f> &positions )
{
  assert( positions.size() == mSkeleton.size() );
  mSkeleton = positions;
  buildOutline();
}

void FatLine3d::buildOutline()
{
  Vec3f eye_axis = Vec3f::yAxis();
  Vec3f a, b, c;
  // first vertex
  a = mSkeleton.at( 0 );
  b = mSkeleton.at( 1 );
  c = mSkeleton.at( 2 );
  Vec3f edge = (b - a).normalized();
  Vec3f nextEdge = (c - b).normalized();
  Vec3f tangent = edge.cross( nextEdge );
  Vec3f north = tangent.normalized();
  Vec3f south = -north;
  mOutline.at( 0 ) = a + north;
  mOutline.at( 0 + 1) = a + south;
  // intermediate vertices
  for( int i = 1; i < mSkeleton.size() - 1; ++i )
  {
    a = mSkeleton.at(i - 1);
    b = mSkeleton.at(i);
    c = mSkeleton.at(i + 1);
    edge = (b - a).normalized();
    Vec3f nextEdge = (c - b).normalized();
    Vec3f tangent = edge.cross( nextEdge );
    north = tangent.normalized();
    south = -north;
    mOutline.at( i * 2 ) = b + north;
    mOutline.at( i * 2 + 1 ) = b + south;
  }
  // last vertex
  c = mSkeleton.back();
  edge = nextEdge;
  nextEdge = (c - b).normalized();
  tangent = edge.cross( nextEdge );
  north = tangent.normalized();
  south = -north;
  int end = mSkeleton.size() - 1;
  mOutline.at( end * 2 ) = c + north;
  mOutline.at( end * 2 + 1) = c + south;

  auto iter = mVbo.mapVertexBuffer();
  for( const auto &loc : mOutline )
  {
    iter.setPosition( loc );
    ++iter;
  }
}

void FatLine3d::draw()
{
  gl::draw( mVbo );
}

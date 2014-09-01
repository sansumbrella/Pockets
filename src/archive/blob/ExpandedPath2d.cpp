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

#include "ExpandedPath2d.h"
#include "cinder/CinderMath.h"
#include "cinder/gl/gl.h"

using namespace pockets;
using namespace std;
using namespace ci;

ExpandedPath2d::ExpandedPath2d( size_t length )
{
  mSkeleton.assign( length, vec2( 0 ) );
  mOutline.assign( length * 2, vec2( 0 ) );

  vector<vec2> texcoords;
  for ( int i = 0; i != length; ++i )
  {
    float factor = i / (length - 1.0f);
    texcoords.push_back( vec2( factor, 0 ) ); // north
    texcoords.push_back( vec2( factor, 1 ) ); // south
  }

  // if( !mVbo )
  // {
  //   gl::VboMesh::Layout layout;
  //   layout.setDynamicPositions();
  //   layout.setStaticTexCoords2d();
  //   mVbo = gl::VboMesh( length * 2, 0, layout, GL_TRIANGLE_STRIP );
  //   mVbo.bufferTexCoords2d( 0, texcoords );
  //   auto iter = mVbo.mapVertexBuffer();
  //   for( const auto &loc : mOutline )
  //   {
  //     iter.setPosition( vec3(loc, 0) );
  //     ++iter;
  //   }
  // }
}

ExpandedPath2d::~ExpandedPath2d()
{}

void ExpandedPath2d::shiftFront( const vec2 &pos )
{
  mSkeleton.push_front( pos );
  mSkeleton.pop_back();
  mDirty = true;
}

void ExpandedPath2d::shiftBack( const vec2 &pos )
{
  mSkeleton.push_back( pos );
  mSkeleton.pop_front();
  mDirty = true;
}

void ExpandedPath2d::buildOutline()
{
  vec2 a, b, c;
  const float last_index = mSkeleton.size() - 1.0f;
  // first vertex
  a = mSkeleton.at( 0 );
  b = mSkeleton.at( 1 );
  vec2 edge = (b - a);
  vec2 tangent = vec2( -edge.y, edge.x );
  vec2 north = tangent.normalized() * getHalfWidth( 0 );
  mOutline.at( 0 ) = a + north;
  mOutline.at( 0 + 1) = a - north;
  // remaining vertices
  for( int i = 1; i < mSkeleton.size() - 1; ++i )
  {
    a = mSkeleton.at(i - 1);
    b = mSkeleton.at(i);
    c = mSkeleton.at(i + 1);
    edge = ((b - a).normalized() + (c - b).normalized()) * 0.5;
    tangent = vec2( -edge.y, edge.x );
    north = tangent.normalized() * getHalfWidth( i / last_index );
    mOutline.at( i * 2 ) = b + north;
    mOutline.at( i * 2 + 1 ) = b - north;
  }
  // final vertex
  c = mSkeleton.back();
  edge = mClosed ? (mSkeleton.at( 1 ) - c) : (c - b);
  tangent = vec2( -edge.y, edge.x );
  north = tangent.normalized() * getHalfWidth( 1.0f );
  size_t end = mSkeleton.size() - 1;
  mOutline.at( end * 2 ) = c + north;
  mOutline.at( end * 2 + 1 ) = c - north;

  // assign positions to VBO
  // auto iter = mVbo.mapVertexBuffer();
  // for( const auto &loc : mOutline )
  // {
  //   iter.setPosition( vec3(loc, 0) );
  //   ++iter;
  // }
}

float ExpandedPath2d::getHalfWidth( float t )
{
  return mShapeFn( t ) * mLineHalfWidth;
}

void ExpandedPath2d::draw()
{
//  gl::draw( mVbo );
  if( mDirty )
  {
    buildOutline();
    mDirty = false;
  }
  glEnableClientState( GL_VERTEX_ARRAY );
  glVertexPointer( 2, GL_FLOAT, 0, &mOutline[0] );
  glEnableClientState( GL_TEXTURE_COORD_ARRAY );
  glTexCoordPointer( 2, GL_FLOAT, 0, &mTexCoords[0] );
  glDrawArrays( GL_TRIANGLE_STRIP, 0, mOutline.size() );
  glDisableClientState( GL_VERTEX_ARRAY );
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

/*
 * Copyright (c) 2013 David Wicks
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

#include "ExpandedLine2d.h"
using namespace pockets;
using namespace ci;

ExpandedLine2d::ExpandedLine2d( const vec2 &begin, const vec2 &end ):
mBegin( begin )
, mRay( end - begin )
{
  buildTexCoords();
  buildOutline( begin, end );
}

ExpandedLine2d::~ExpandedLine2d()
{}

void ExpandedLine2d::setEndPoints(const ci::vec2 &begin, const ci::vec2 &end)
{
  mBegin = begin;
  mRay = end - begin;
  buildOutline( begin, end );
}

void ExpandedLine2d::matchSprite(const pockets::SpriteData &sprite)
{
  mWidth = sprite.getSize().y / 2;
  buildTexCoords( sprite.getTextureBounds() );
}

void ExpandedLine2d::scaleLength( float scale )
{
  mScale = scale;
  buildOutline( mBegin, mBegin + mRay * mScale );
}

void ExpandedLine2d::scaleLengthInverse(float scale)
{
  mScale = scale;
  buildOutline( mBegin + mRay - mRay * scale, mBegin + mRay );
}

void ExpandedLine2d::buildOutline( const vec2 &begin, const vec2 &end )
{
  vec2 cap = mRay.normalized() * mWidth;
  vec2 N( -cap.y, cap.x );
  vec2 S = -N;
  vec2 NW = N - cap;
  vec2 NE = N + cap;
  vec2 SE = -NW;
  vec2 SW = -NE;

  mPositions.at(0) = begin + SW;
  mPositions.at(1) = begin + NW;
  mPositions.at(2) = begin + S;
  mPositions.at(3) = begin + N;
  mPositions.at(4) = end + S;
  mPositions.at(5) = end + N;
  mPositions.at(6) = end + SE;
  mPositions.at(7) = end + NE;
}

void ExpandedLine2d::buildTexCoords( const ci::Rectf &bounds )
{
  float mid_s = bounds.getX1() + bounds.getWidth() * 0.5f;
  mTexCoords.at(0) = vec2( bounds.getX1(), bounds.getY2() );
  mTexCoords.at(1) = vec2( bounds.getX1(), bounds.getY1() );
  mTexCoords.at(2) = vec2( mid_s, bounds.getY2() );
  mTexCoords.at(3) = vec2( mid_s, bounds.getY1() );
  mTexCoords.at(4) = vec2( mid_s, bounds.getY2() );
  mTexCoords.at(5) = vec2( mid_s, bounds.getY1() );
  mTexCoords.at(6) = vec2( bounds.getX2(), bounds.getY2() );
  mTexCoords.at(7) = vec2( bounds.getX2(), bounds.getY1() );
}

void ExpandedLine2d::render()
{
  if( mScale > 0 )
  {
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 2, GL_FLOAT, 0, &mPositions[0] );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer( 2, GL_FLOAT, 0, &mTexCoords[0] );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, mPositions.size() );
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  }
}

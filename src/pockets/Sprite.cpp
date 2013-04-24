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

#include "Sprite.h"

using namespace pockets;
using namespace ci;
using namespace std;

SpriteData::SpriteData( const Vec2i &size, const Rectf &texture_bounds, const Vec2i &registration_point ):
mSize( app::toPoints( size ) )
, mTextureBounds( texture_bounds )
, mRegistrationPoint( app::toPoints( registration_point ) )
{}

Sprite::Sprite( const SpriteData &sprite ):
mData( sprite )
{
  Rectf tex_coord_rect = sprite.getTextureBounds();
  Rectf position_rect( Vec2f::zero(), sprite.getSize() );
  updatePositions( position_rect );
  updateTexCoords( tex_coord_rect );
}

Sprite::~Sprite()
{}

void Sprite::updatePositions(const ci::Rectf &positions)
{
  Rectf position_rect = positions - mData.getRegistrationPoint();
  mPositions[0*2+0] = position_rect.getX2();
	mPositions[0*2+1] = position_rect.getY1();

	mPositions[1*2+0] = position_rect.getX1();
	mPositions[1*2+1] = position_rect.getY1();

  mPositions[2*2+0] = position_rect.getX2();
	mPositions[2*2+1] = position_rect.getY2();

  mPositions[3*2+0] = position_rect.getX1();
	mPositions[3*2+1] = position_rect.getY2();
}

void Sprite::updateTexCoords(const ci::Rectf &tex_coord_rect)
{
  mTexCoords[0*2+0] = tex_coord_rect.getX2();
	mTexCoords[0*2+1] = tex_coord_rect.getY1();

	mTexCoords[1*2+0] = tex_coord_rect.getX1();
	mTexCoords[1*2+1] = tex_coord_rect.getY1();

  mTexCoords[2*2+0] = tex_coord_rect.getX2();
	mTexCoords[2*2+1] = tex_coord_rect.getY2();

  mTexCoords[3*2+0] = tex_coord_rect.getX1();
	mTexCoords[3*2+1] = tex_coord_rect.getY2();
}

void Sprite::clipBy(const ci::Rectf &bounding_rect)
{
  Rectf sprite_bounds( Vec2f::zero(), mData.getSize() );
  sprite_bounds.offset( mLocus.getLoc() - mData.getRegistrationPoint() );
  Rectf clipped_size = sprite_bounds.getClipBy( bounding_rect );
  clipped_size.offset( mData.getRegistrationPoint() - mLocus.getLoc() );
  Rectf portion( clipped_size.getX1() / mData.getSize().x, clipped_size.getY1() / mData.getSize().y
                , clipped_size.getX2() / mData.getSize().x, clipped_size.getY2() / mData.getSize().y );
  setRegion( portion );
}

void Sprite::setRegion(const ci::Rectf &portion)
{
  Rectf tex_coords;
  tex_coords.x1 = lerp( mData.getTextureBounds().getX1(), mData.getTextureBounds().getX2(), portion.getX1() );
  tex_coords.x2 = lerp( mData.getTextureBounds().getX1(), mData.getTextureBounds().getX2(), portion.getX2() );
  tex_coords.y1 = lerp( mData.getTextureBounds().getY1(), mData.getTextureBounds().getY2(), portion.getY1() );
  tex_coords.y2 = lerp( mData.getTextureBounds().getY1(), mData.getTextureBounds().getY2(), portion.getY2() );

  Vec2f br( mLocus.getLoc() + mData.getSize() );
  Vec2f tl( mLocus.getLoc() );
  Rectf positions;
  positions.x1 = lerp( tl.x, br.x, portion.getX1() );
  positions.x2 = lerp( tl.x, br.x, portion.getX2() );
  positions.y1 = lerp( tl.y, br.y, portion.getY1() );
  positions.y2 = lerp( tl.y, br.y, portion.getY2() );

  updatePositions( positions );
  updateTexCoords( tex_coords );
}

void Sprite::render()
{
	gl::pushModelView();
	gl::multModelView( mLocus );
  glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, &mPositions[0] );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, 0, &mTexCoords[0] );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	gl::popModelView();
}

SpriteAnimation::SpriteAnimation():
mDuration( 0 )
{}

SpriteAnimation::SpriteAnimation( const vector<SpriteWithTiming> &sprites ):
mData( sprites )
, mDuration( sprites.back().finish )
{}

void SpriteAnimation::addFrame(const SpriteData &sprite, float duration)
{
  mData.push_back( { sprite, mDuration, mDuration + duration } );
  mDuration = mData.back().finish;
}

void SpriteAnimation::nextFrame()
{
  size_t next = mCurrentIndex;
  next++;
  if( next >= mData.size() )
  {
    if( mLooping )
    {
      mTime -= mDuration;
      next = 0;
    }
    else
    {
      next = mCurrentIndex;
    }
  }

  if( next != mCurrentIndex )
  {
    mCurrentIndex = next;
    Rectf tex_coord_rect = currentSprite().getTextureBounds();
    Rectf position_rect( Vec2f::zero(), currentSprite().getSize() );
    position_rect -= currentSprite().getRegistrationPoint();

    mPositions[0*2+0] = position_rect.getX2(); mTexCoords[0*2+0] = tex_coord_rect.getX2();
    mPositions[0*2+1] = position_rect.getY1(); mTexCoords[0*2+1] = tex_coord_rect.getY1();

    mPositions[1*2+0] = position_rect.getX1(); mTexCoords[1*2+0] = tex_coord_rect.getX1();
    mPositions[1*2+1] = position_rect.getY1(); mTexCoords[1*2+1] = tex_coord_rect.getY1();

    mPositions[2*2+0] = position_rect.getX2(); mTexCoords[2*2+0] = tex_coord_rect.getX2();
    mPositions[2*2+1] = position_rect.getY2(); mTexCoords[2*2+1] = tex_coord_rect.getY2();

    mPositions[3*2+0] = position_rect.getX1(); mTexCoords[3*2+0] = tex_coord_rect.getX1();
    mPositions[3*2+1] = position_rect.getY2(); mTexCoords[3*2+1] = tex_coord_rect.getY2();
  }
}

void SpriteAnimation::update( float deltaTime )
{
  mTime += deltaTime;
  if( mTime >= currentData().finish )
  {
    nextFrame();
  }
}

void SpriteAnimation::render()
{
  gl::pushModelView();
	gl::multModelView( mLocus );
  glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, &mPositions[0] );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, 0, &mTexCoords[0] );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	gl::popModelView();
}

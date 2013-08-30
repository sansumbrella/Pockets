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

#include "SpriteAnimation.h"

#include "cinder/Json.h"
#include "SpriteSheet.h"

using namespace pockets;
using namespace cinder;

SpriteAnimation::SpriteAnimation()
{}

void SpriteAnimation::loadAnimationJson( const ci::JsonTree &json, const SpriteSheetRef sheet )
{
  try
  {
    setFrameRate( json.getChild("fps").getValue<float>() );
    auto frames = json.getChild("frames");
    for( auto &child : frames.getChildren() )
    {
      addFrame( sheet->getSpriteData( child[0].getValue() ), child[1].getValue<float>() );
    }
  }
  catch( JsonTree::Exception &exc )
  {
    std::cout << __FUNCTION__ << " error: " << exc.what() << std::endl;
  }
}

SpriteAnimation& SpriteAnimation::addFrame( const SpriteData &sprite, float hold )
{
  mData.push_back( { sprite, hold } );
  return *this;
}

void SpriteAnimation::step( float deltaTime )
{
  mHold += deltaTime;
  if( mHold > mFrameDuration * currentData().hold )
  { // move to next frame
    stepFrame( 1 );
    mHold = 0.0f;
  }
  else if ( mHold < 0.0f )
  { // step back a frame
    stepFrame( -1 );
    mHold = mFrameDuration * currentData().hold;
  }
}

void SpriteAnimation::stepFrame( int frames )
{
  int next = mCurrentIndex + frames;
  if( next >= static_cast<int>( mData.size() ) )
  { // handle wraparound at end
    next = mLooping ? 0 : mData.size() - 1;
  }
  else if( next < 0 )
  { // handle wraparound at beginning
    next = mLooping ? mData.size() - 1 : 0;
  }
  if( next != mCurrentIndex )
  { // the frame index has changed, update display
    mCurrentIndex = next;
    updateGraphics();
  }
}

void SpriteAnimation::updateGraphics()
{
  const auto sprite = currentData().sprite;
  Rectf tex_coord_rect = sprite.getTextureBounds();
  Rectf position_rect( Vec2f::zero(), sprite.getSize() );
  position_rect -= sprite.getRegistrationPoint();

  mPositions[0*2+0] = position_rect.getX2(); mTexCoords[0*2+0] = tex_coord_rect.getX2();
  mPositions[0*2+1] = position_rect.getY1(); mTexCoords[0*2+1] = tex_coord_rect.getY1();

  mPositions[1*2+0] = position_rect.getX1(); mTexCoords[1*2+0] = tex_coord_rect.getX1();
  mPositions[1*2+1] = position_rect.getY1(); mTexCoords[1*2+1] = tex_coord_rect.getY1();

  mPositions[2*2+0] = position_rect.getX2(); mTexCoords[2*2+0] = tex_coord_rect.getX2();
  mPositions[2*2+1] = position_rect.getY2(); mTexCoords[2*2+1] = tex_coord_rect.getY2();

  mPositions[3*2+0] = position_rect.getX1(); mTexCoords[3*2+0] = tex_coord_rect.getX1();
  mPositions[3*2+1] = position_rect.getY2(); mTexCoords[3*2+1] = tex_coord_rect.getY2();
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

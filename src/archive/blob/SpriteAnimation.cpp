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
  if( mData.size() == 1 ){ updateGraphics(); }  // prep display of first frame
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
  Rectf position_rect( vec2( 0 ), sprite.getSize() );
  position_rect -= sprite.getRegistrationPoint();

  mVertices[0].position = { position_rect.getX2(), position_rect.getY1() };
  mVertices[1].position = { position_rect.getX1(), position_rect.getY1() };
  mVertices[2].position = { position_rect.getX2(), position_rect.getY2() };
  mVertices[3].position = { position_rect.getX1(), position_rect.getY2() };

  mVertices[0].tex_coord = { tex_coord_rect.getX2(), tex_coord_rect.getY1() };
  mVertices[1].tex_coord = { tex_coord_rect.getX1(), tex_coord_rect.getY1() };
  mVertices[2].tex_coord = { tex_coord_rect.getX2(), tex_coord_rect.getY2() };
  mVertices[3].tex_coord = { tex_coord_rect.getX1(), tex_coord_rect.getY2() };
}

void SpriteAnimation::render()
{
  gl::pushModelView();
	gl::multModelView( mLocus );
  glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, sizeof( Vertex2d ), &mVertices[0].position.x );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex2d ), &mVertices[0].tex_coord.x );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, mVertices.size() );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	gl::popModelView();
}

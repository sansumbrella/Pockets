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

#pragma once
#include "SimpleRenderer.h"
#include "SpriteSheet.h"
#include "Locus2d.h"

/**
 An renderable Sprite
*/
namespace pockets
{
  typedef SpriteSheet::SpriteData SpriteData;

  class Sprite : public SimpleRenderer::IRenderable
  {
  public:
    Sprite( const SpriteData &data );
    ~Sprite();
    void render();
    Locus2d& getLocus(){ return mLocus; }
  private:
    SpriteData                mData;
    Locus2d                   mLocus;
    std::array<GLfloat, 8>    mPositions;
    std::array<GLfloat, 8>    mTexCoords;
  };

  struct SpriteWithTiming
  {
    SpriteWithTiming( const SpriteData &sprite, float start, float end );
    SpriteData sprite;
    float start;
    float finish;
  };

  class SpriteAnimation : public SimpleRenderer::IRenderable
  {
  public:
    SpriteAnimation( const std::vector<SpriteWithTiming> &sprites );
    const SpriteData& currentSprite(){ return mCurrentData->sprite; }
    const SpriteWithTiming& currentTiming(){ return *mCurrentData; }
    //! add a frame at end of animation
    void addFrame( const SpriteData &sprite, float duration );
    //! update the animation
    void update( float deltaTime );
    //! render the current frame of animation
    void render();
  private:
    typedef std::vector<SpriteWithTiming>::iterator DataIter;
    Locus2d                             mLocus;
    std::vector<SpriteWithTiming>       mData;
    DataIter                            mCurrentData;
    float                               mTime = 0;
    float                               mDuration;
    bool                                mLooping = true;
    std::array<GLfloat, 8>              mPositions;
    std::array<GLfloat, 8>              mTexCoords;
    void nextFrame();
  };

} // pockets::

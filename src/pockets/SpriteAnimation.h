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
#include "Sprite.h"

namespace pockets
{
  /**
   SpriteAnimations hold a collection of SpriteData
   and renders the appropriate Sprite given the current time.
   */
  typedef std::shared_ptr<class SpriteAnimation> SpriteAnimationRef;
  class SpriteAnimation // : public TriangleRenderer::IRenderable
  {
    struct Drawing{
      SpriteData  sprite;
      float       hold; // frames to hold
    };
  public:
    SpriteAnimation();
    void      setFrameRate( float frames_per_second ){ mFrameDuration = 1.0f / frames_per_second; }
    //! add a frame at end of animation
    void      addFrame( const SpriteData &sprite, float duration );
    //! update the animation forward or backward in time
    void      step( float deltaTime );
    //! render the current frame of animation
    void      render();
    //! set the animation to loop (on by default)
    void      setLooping( bool doLoop ) { mLooping = doLoop; }
    Locus2d&  getLocus(){ return mLocus; }
    const Drawing& currentData(){ return mData.at(mCurrentIndex); }
  private:
    Locus2d                    mLocus;
    std::vector<Drawing>       mData;
    int                        mCurrentIndex = 0;
    float                      mHold = 0.0f; // time spent on this frame
    float                      mFrameDuration = 1.0f / 60.0f;
    bool                       mLooping = true;
    std::array<GLfloat, 8>     mPositions;
    std::array<GLfloat, 8>     mTexCoords;
    void stepFrame( int frames );
    void updateGraphics();
  };

} // pockets::

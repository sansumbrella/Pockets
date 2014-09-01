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

// forward decl
namespace cinder
{
  class JsonTree;
}

namespace pockets
{
  // forward decl
  typedef std::shared_ptr<class SpriteSheet> SpriteSheetRef;

  /**
   SpriteAnimations hold a collection of SpriteData
   and renders the appropriate Sprite given the current time.
   */
  typedef std::shared_ptr<class SpriteAnimation> SpriteAnimationRef;
  class SpriteAnimation
  {
    struct Drawing{
      SpriteData  sprite;
      float       hold; // frames to hold
    };
  public:
    SpriteAnimation();
    //! Load an animation from \a json, using sprites from \a sheet
    //! See the SpriteAnimation sample assets for a working json file
    //! Expects JSON in the format:
    //! {"fps":number, "frames":[ ["id", number_of_frames_held] ]}
    void      loadAnimationJson( const ci::JsonTree &json, const SpriteSheetRef sheet );
    //! Set the frame rate for the entire animation
    //! Defaults to 24
    SpriteAnimation&  setFrameRate( float frames_per_second ){ mFrameDuration = 1.0f / frames_per_second; return *this; }
    //! Add a new frame at the end of the animation, held for \a hold frames
    //! returns self to make adding lots of frames easier
    SpriteAnimation&  addFrame( const SpriteData &sprite, float hold );
    std::vector<Vertex2d> getVertices() { return { mVertices[0], mVertices[1], mVertices[2], mVertices[3] }; }
    //! step the animation forward or backward in time
    void      step( float deltaTime );
    //! render the current frame of animation
    void      render();
    //! set the animation to loop (on by default)
    void      setLooping( bool doLoop ) { mLooping = doLoop; }
    //! get the screen position of the animation
    Locus2d&  getLocus(){ return mLocus; }
    //! get the sprite data for the current frame
    const Drawing& currentData() const { return mData.at(mCurrentIndex); }
    ci::ivec2 getSize() const { return currentData().sprite.getSize(); }
  private:
    Locus2d               mLocus;
    std::vector<Drawing>  mData;
    int                   mCurrentIndex = 0; // this limits us to 32k frames per animation...
    float                 mHold = 0.0f;      // time spent on this frame
    float                 mFrameDuration = 1.0f / 24.0f;
    bool                  mLooping = true;
    std::array<Vertex2d, 4> mVertices;
    void stepFrame( int frames );
    void updateGraphics();
  };
} // pockets::

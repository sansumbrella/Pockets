/*
 * Copyright (c) 2013 David Wicks, sansumbrella.com
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
#include "pockets/puptent/PupTent.h"
#include "pockets/puptent/TextureAtlas.h"
#include "pockets/CollectionUtilities.hpp"

namespace cinder
{
  class JsonTree;
}

namespace pockets
{ namespace puptent
{
  /**
   SpriteAnimation:
   A playing animation
  */
  typedef size_t AnimationId;
  typedef std::shared_ptr<class SpriteAnimation>  SpriteAnimationRef;
  typedef std::function<void (SpriteAnimationRef)> SpriteAnimationCallback;
  struct SpriteAnimation : Component<SpriteAnimation>
  {
    SpriteAnimation() = default;
    SpriteAnimation( AnimationId animation ):
    animation( animation )
    {}
    AnimationId               animation = 0;
    bool                      looping = true;
    float                     hold = 0.0f;      // time spent on this frame
    float                     rate = 1.0f;      // multiplayer for playback rate
    int                       current_index = 0;
    // called when animation is completed (at the end of every loop if looping)
    SpriteAnimationCallback   finish_fn = nullptr;
  };

  typedef std::shared_ptr<class SpriteAnimationSystem> SpriteAnimationSystemRef;
  /**
   SpriteAnimationSystem:
   Plays back SpriteAnimations
   Updates a RenderMesh component with the current animation frame
   Assumes that whatever renderer will bind the correct texture for display
   */
  class SpriteAnimationSystem : public System<SpriteAnimationSystem>, public Receiver<SpriteAnimationSystem>
  {
  public:
    struct Drawing
    {
      Drawing( const SpriteData &drawing=SpriteData{}, float hold=1.0f ):
      drawing( drawing ),
      hold( hold )
      {}
      SpriteData      drawing;  // size and texture information
      float           hold;     // frames to hold
    };
    struct Animation
    {
      std::string           name;
      std::vector<Drawing>  drawings;
      float                 frame_duration;
    };
    SpriteAnimationSystem( TextureAtlasRef atlas, const ci::JsonTree &animations );
    //! create a new animation system drawing from \a atlas with \a animations
    static SpriteAnimationSystemRef create( TextureAtlasRef atlas, const ci::JsonTree &animations );
    //! called by SystemManager to register event handlers
    void configure( EventManagerRef events ) override;
    //! update mesh on sprite creation
    void receive( const ComponentAddedEvent<SpriteAnimation> &event );
    void update( EntityManagerRef es, EventManagerRef events, double dt ) override;
    //! Create a component to play \a animation_name
    //! To display the animation properly, you will need to assign new component's mesh
    SpriteAnimationRef createSpriteAnimation( const std::string &animation_name ) const;
    //! Create a component to play \a animation_id
    SpriteAnimationRef createSpriteAnimation( AnimationId animation_id ) const;
    //! Returns the id of \a animation_name
    AnimationId        getAnimationId( const std::string &animation_name ) const;
    //! Add a new animation to the system's list of animations
    //! You can then create components that reference your animation
    void addAnimation( const std::string &name, const Animation &animation );
  private:
    TextureAtlasRef                     _atlas;
    // name : index into mAnimations
    std::map<std::string, AnimationId>  _animation_ids;
    std::vector<Animation>              _animations;
  };

} // puptent::
} // pockets::

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
#include "puptent/PupTent.h"

namespace pockets
{ namespace puptent
{
  struct ParticleEmitter : Component<ParticleEmitter>
  {
    ci::Vec3f   direction;  // direction in which to fire particles
    float       rate;
    float       hold;
    // called whenever a particle is created from this emitter
    // use to apply desired RenderMesh and additional behavior components
    std::function<void (Entity)>  build_fn;
  };

  /**
   Particle:
   A bag of attributes for performing verlet integration
   Any custom movement systems should just change the Locus component values.
   The ParticleSystem will integrate based on the sum of changes.
   To set a particle and force it to stay still, also set previous positions.
   */
  typedef std::shared_ptr<class Particle> ParticleRef;
  struct Particle : Component<Particle>
  {
    Particle( LocusRef locus );
    Particle() {}
    float     friction = 0.5f;
    float     rotation_friction = 0.5f;
    float     scale_friction = 0.5f;
    ci::Vec2f p_position = ci::Vec2f::zero();
    float     p_rotation = 0.0f;
    float     p_scale = 1.0f;
  };

  /**
   ParticleSystem:
   Simple particle system; a specialized movement system
   Each "particle" is an entity with Particle and Locus components
   This lets us integrate the particles with the BatchRenderSystem so that they
   can be layered interwoven with other entities.
   */
  struct ParticleSystem : public System<ParticleSystem>, Receiver<ParticleSystem>
  {
    void configure( EventManagerRef events ) override;
    void update( EntityManagerRef es, EventManagerRef events, double dt ) override;
    void receive( const ComponentAddedEvent<Particle> &event );
    void receive( const ComponentRemovedEvent<Particle> &event );
    void receive( const ComponentAddedEvent<ParticleEmitter> &event );
    void receive( const ComponentRemovedEvent<ParticleEmitter> &event );
    void receive( const EntityDestroyedEvent &event );
  private:
    std::vector<Entity>       mParticles;
    std::vector<Entity>       mEmitters;
    ci::Vec3f                 mGravity;
    bool                      mHandleEvents;
  };
} // puptent::
} // pockets::
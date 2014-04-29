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

#include "pockets/puptent/ParticleSystem.h"
#include "pockets/puptent/Locus.h"
#include "pockets/CollectionUtilities.hpp"

using namespace pockets;
using namespace puptent;
using namespace cinder;

Particle::Particle( LocusRef locus ):
p_position( locus->getPosition() ),
p_rotation( locus->getRotation() ),
p_scale( locus->getScale() )
{}


void ParticleSystem::configure( EventManagerRef events )
{
  events->subscribe<ComponentAddedEvent<Particle>>( *this );
  events->subscribe<ComponentRemovedEvent<Particle>>( *this );
  events->subscribe<ComponentAddedEvent<ParticleEmitter>>( *this );
  events->subscribe<ComponentRemovedEvent<ParticleEmitter>>( *this );
  events->subscribe<EntityDestroyedEvent>( *this );
}

void ParticleSystem::receive( const ComponentAddedEvent<Particle> &event )
{
  mParticles.push_back( event.entity );
}

void ParticleSystem::receive( const ComponentRemovedEvent<Particle> &event )
{
  vector_remove( &mParticles, event.entity );
}

void ParticleSystem::receive( const ComponentAddedEvent<ParticleEmitter> &event )
{
  mEmitters.push_back( event.entity );
}

void ParticleSystem::receive( const ComponentRemovedEvent<ParticleEmitter> &event )
{
  vector_remove( &mEmitters, event.entity );
}

void ParticleSystem::receive( const EntityDestroyedEvent &event )
{
  auto entity = event.entity;
  if( entity.component<ParticleEmitter>() )
  {
    vector_remove( &mEmitters, entity );
  }
  if( entity.component<Particle>() )
  {
    vector_remove( &mParticles, entity );
  }
}

void ParticleSystem::update( EntityManagerRef es, EventManagerRef events, double dt )
{
  for( auto entity : mEmitters )
  {
    auto emitter = entity.component<ParticleEmitter>();
    auto loc = entity.component<Locus>();
    Entity e = es->create();
    auto p = e.assign<Particle>();
    auto l = e.assign<Locus>();
    if( emitter->build_fn )
    {
      emitter->build_fn( e );
    }
  }

  for( auto entity : mParticles )
  {
    // Perform verlet integration
    ParticleRef p = entity.component<Particle>();
    LocusRef l = entity.component<Locus>();
    if( l )
    {
      Vec2f position = l->position;
      Vec2f velocity = position - p->p_position;
      l->position = position + velocity * p->friction;
      p->p_position = position;

      float rotation = l->rotation;
      float r_vel = rotation - p->p_rotation;
      l->rotation = rotation + r_vel * p->rotation_friction;
      p->p_rotation = rotation;

      float scale = l->scale;
      float s_vel = scale - p->p_scale;
      l->scale = scale + s_vel * p->scale_friction;
      p->p_scale = scale;
    }
  }
}

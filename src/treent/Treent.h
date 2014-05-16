/*
  (Ref) 2014  (om
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

#include "entityx/entityx.h"

/**
  Treent marries entity system's non-hierarchical, non-homogenous
  structure with a hierarchical, semi-homogenous structure.

  It provides:
  - a scene graph (Tree) for organizing spatial entities.
  - entity system hooks on every node.
  - RAII memory management of entities and components. When TreentNodes fall out of scope, their entities are destroyed.
  - Convenient method for defining composite objects (your constructor). You can add children and/or components at runtime.
 */
namespace treent
{
  using namespace entityx;
  typedef std::shared_ptr<class TreentNode> TreentNodeRef;
  typedef std::shared_ptr<EventManager>     EventManagerRef;
  typedef std::shared_ptr<EntityManager>    EntityManagerRef;
  typedef std::shared_ptr<SystemManager>    SystemManagerRef;

  typedef std::shared_ptr<struct LocationComponent> LocationComponentRef;

  struct Treent
  {
    EventManagerRef   events   = EventManagerRef( new EventManager() );
    EntityManagerRef  entities = EntityManagerRef( new EntityManager( events ) );
    SystemManagerRef  systems  = SystemManagerRef( new SystemManager( entities, events ) );
    TreentNodeRef createRoot();
  };
}

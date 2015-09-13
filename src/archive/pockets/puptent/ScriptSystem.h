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

#ifndef ENABLE_LUA
#define ENABLE_LUA 0
#endif

#include "pockets/puptent/PupTent.h"
#include "cinder/app/TouchEvent.h"
#include "cinder/app/KeyEvent.h"
#include "cinder/Filesystem.h"

#if ENABLE_LUA
#include "selene.h"

struct lua_State;
#endif

namespace pockets
{ namespace puptent
  {
#if ENABLE_LUA
    //
    //  Dynamic Script Behaviors
    //

    /**
     ScriptComponent:
     Runs an arbitrary Lua script on every update cycle.
     Use for behavior unique to an entity, like user-controlled entities,
     special items, synching timeline animations, or temporary behaviors.

     We are going to use Selene to handle the initial Lua binding.
     */

    // ScriptFn receives self entity, entity manager for world queries, event manager, and timestep
    struct ScriptComponent : Component<ScriptComponent>
    {
      ci::fs::path script_path;
      std::string  script;
      const char*  table; // where this objects functions are stored in lua
    };

    /**
     ScriptSystem:
     Executes arbitrary code on a component.
     Passes in entity information.
     */
    struct ScriptSystem : public System<ScriptSystem>, Receiver<ScriptSystem>
    {
      ScriptSystem();
      ~ScriptSystem();
      void  configure( EventManagerRef event_manager ) override;
      //! gather scripts and execute them
      void  update( EntityManagerRef es, EventManagerRef events, double dt ) override;
      //! TODO: load the specified script when component is added
      void  receive( const ComponentAddedEvent<ScriptComponent> &event ) {}
      void  receive( const ComponentRemovedEvent<ScriptComponent> &event ) {}
    private:
      void  handleLuaError( int error );
      lua_State *L;
    };
#endif // ENABLE_LUA
    //
    //  C++ driven behaviors
    //

    // update_fn receives self entity and timestep
    struct CppScriptComponent : Component<CppScriptComponent>
    {
      CppScriptComponent( const std::function<void (Entity, double)> &fn )
      : update_fn( fn )
      {}
      std::function<void (Entity, double)>  update_fn;
    };

    struct CppScriptSystem : public System<CppScriptSystem>
    {
      void  update( EntityManagerRef es, EventManagerRef events, double dt ) override;
    };
  } // puptent::
} // pockets::

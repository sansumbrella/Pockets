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

#include "ScriptSystem.h"

extern "C"{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

using namespace std;
using namespace pockets::puptent;

// test function for scripting
int l_cout( lua_State *L ){
  double d = luaL_checknumber( L, 1 );
  cout << "Number from lua: " << d << endl;
  return 0; // number of results
}

// list of functions to register
// this null-terminated c-array is how lua wants it, so don't fight it
luaL_Reg l_lib[] = {
  { "cout", l_cout },
  { NULL, NULL } // sentinel
};

ScriptSystem::ScriptSystem()
{
  L = luaL_newstate();
  // open all standard lua libraries
  luaL_openlibs( L );
  // create our own library of lua functions
  luaL_newlib( L, l_lib );
  // assign them to the "su" namespace
  lua_setglobal( L, "su" );

  int error = luaL_loadstring( L, "a = 1.5" ) || lua_pcall( L, 0, 0, 0 );
  handleLuaError( error );
}

ScriptSystem::~ScriptSystem()
{
  lua_close( L );
}

void ScriptSystem::update(shared_ptr<entityx::EntityManager> es, shared_ptr<entityx::EventManager> events, double dt)
{
  for( auto entity : es->entities_with_components<ScriptComponent>() )
  {
    auto script = entity.component<ScriptComponent>();
    script->update( entity, dt );
  }

  int error = luaL_loadstring( L, "a = a + 2" ) || lua_pcall( L, 0, 0, 0 );
  handleLuaError( error );
  error = luaL_loadstring( L, "su.cout( a )" ) || lua_pcall( L, 0, 0, 0 );
  handleLuaError( error );
}

void ScriptSystem::handleLuaError( int error )
{
  if( error )
  { // get the last item on the stack (negative indices work from the end)
    cinder::app::console() << "Lua Script Error: " << lua_tostring( L, -1 ) << endl;
    // pop the error off the end of the stack so we can continue
    lua_pop( L, 1 );
  }
}


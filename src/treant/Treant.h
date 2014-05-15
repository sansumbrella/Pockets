/*
 * Copyright (c) 2014 David Wicks, sansumbrella.com
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

#include "entityx.h"

/**
 Provides a hierachical structure for organizing spatial entities.
 Provides implicit memory management of entities by destroying them
 when your treant handle goes out of scope.
 Provides an easy way to define entities and groups of entities by
 subclassing TreantNode and adding components and/or child entities.
 */
namespace treant
{
  using namespace entityx;
  typedef std::shared_ptr<class TreantNode> TreantNodeRef;
	struct Treant
	{
		std::shared_ptr<EventManager>	events	 = std::shared_ptr<EventManager>( new EventManager() );
		std::shared_ptr<EntityManager>	entities = std::shared_ptr<EntityManager>( new EntityManager( events ) );
		std::shared_ptr<SystemManager>	systems  = std::shared_ptr<SystemManager>( new SystemManager( entities, events ) );
		TreantNodeRef	createRoot() { return std::make_shared<TreantNode>( entities->create() ); }
	};
}

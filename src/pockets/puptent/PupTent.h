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

#include "pockets/Pockets.h"
#include "entityx/Entity.h"
#include "entityx/System.h"
#include "entityx/Event.h"

#include "cinder/Vector.h"

namespace pockets
{
  /**
  A suite of entities and components for game development using entityx.

  PupTent is meant for use in the development of small games with big hearts.
  It is especially supportive of mixing procedural and hand-drawn 2d art.
  */
  namespace puptent
  {
    using namespace entityx;
    // entityx notation niceties
    typedef std::shared_ptr<EntityManager>			EntityManagerRef;
    typedef std::shared_ptr<EventManager>			EventManagerRef;
    typedef std::shared_ptr<class SystemManager>    SystemManagerRef;
    // forward declarations for common components
    typedef std::shared_ptr<struct Locus>           LocusRef;
    typedef std::shared_ptr<struct RenderMesh>      RenderMeshRef;
    // forward declarations for common utilities
    typedef std::shared_ptr<class TextureAtlas>     TextureAtlasRef;
  } // puptent::
  namespace pt = puptent;
} // pockets::

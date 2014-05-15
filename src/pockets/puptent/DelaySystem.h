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

namespace pockets
{ namespace puptent
{
  /**
   DelayActionComponent:

   Base type for any kind of delayed action you might want to take.
   To use your own delayed action, extend and override ::action();
   Alternatively, use the DelayFunctionComponent which extends this to call
   an arbitrary function after a delay (so you don't need to write a new class).
   */
  struct DelayActionComponent : Component<DelayActionComponent>
  {
    DelayActionComponent( double delay=1.0f ):
    time( delay )
    {}
    double       time;
    virtual void action() {}
  };

  /**
   DelayFunctionComponent:

   Calls the provided function after some time has elapsed.
   Delay is time in seconds.
   */
  struct DelayFunctionComponent : public DelayActionComponent
  {
    DelayFunctionComponent( double delay, std::function<void ()> fn ):
    DelayActionComponent( delay ),
    callback( fn )
    {}
    void action() override { callback(); }
    std::function<void ()> callback;
  };

  /**
   ExpiresSystem:
   Destroys an entity once the time in Expires runs out.
   */
  struct DelaySystem : public System<DelaySystem>
  {
    void update( EntityManager &es, EventManager &events, double dt ) override;
  private:
  };
} // puptent::
} // pockets::
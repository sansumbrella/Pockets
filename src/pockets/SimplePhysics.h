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
#include "pockets/Pockets.h"
#include "cinder/Vector.h"
#include <vector>

/*
 A basic verlet physics engine.
 Useful primarily for UI animation.
*/

namespace pockets
{
	namespace physics
	{
    struct Node
    {
      ci::Vec3f pos;
      ci::Vec3f ppos;
    };

    class Effector
    {
    public:
      //! apply effect to node
      virtual void apply( Node *node ) const = 0;
    };

    class Constraint
    {
    public:
      virtual void apply() const = 0;
    private:
    };

    typedef std::shared_ptr<Node>       NodeRef;
    typedef std::shared_ptr<Constraint> ConstraintRef;
    typedef std::shared_ptr<Effector>   EffectorRef;

    class World
    {
    public:
      //! advance physics one timestep
      void step( double dt );
      void drawNodes();
      NodeRef createNode();
    private:
      std::vector<NodeRef>        mNodes;
      std::vector<ConstraintRef>  mConstraints;
      std::vector<EffectorRef>    mEffectors;
      float                       mFriction = 0.9f;
    };
	}
}

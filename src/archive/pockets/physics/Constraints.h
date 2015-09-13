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
    /**
     A constant function applied twice per world timestep.
     
     Constraints are applied once before integration and once after
     integration by the physics World.
     Useful for specifying springs and other inter-object relationships.
     */
    class Constraint
    {
    public:
      virtual void apply() const = 0;
    };

    // a leash keeps one thing attached to another
    class Lashing : public Constraint
    {
    public:
      //!
      Lashing( NodeRef pet, NodeRef owner, float stiffness=0.5f );
      void apply() const override;
      //! set how strongly the spring returns to rest length
      Lashing& stiffness( float s ) { mStiffness = s; return *this; }
    private:
      NodeRef mPet;
      NodeRef mRock;
      float   mStiffness;
    };

    class Spring : public Constraint
    {
    public:
      //! Create a spring connection between two nodes, with a rest length of the distance between them
      Spring( NodeRef a, NodeRef b, float stiffness=0.9f );
      void apply() const override;
      //! set how strongly the spring returns to rest length
      Spring& stiffness( float s ) { mStiffness = s; return *this; }
      //! sets the spring's rest length; defaults to initial distance between nodes
      Spring& restLength( float l ) { mRestLength = l; return *this; }
    private:
      NodeRef mA;
      NodeRef mB;
      float   mRestLength;
      float   mStiffness;
    };

    class Pin : public Constraint
    {
    public:
      Pin( NodeRef node ): mNode( node )
      {}
      void apply() const override
      { mNode->pos = mNode->ppos; }
    private:
      NodeRef mNode;
    };

    class Range : public Constraint
    {
    public:
      Range( NodeRef node, Vec first, Vec second );
      void apply() const override;
    private:
      NodeRef mNode;
      Vec     mMin;
      Vec     mMax;
    };
	}  // physics::
} // pockets::

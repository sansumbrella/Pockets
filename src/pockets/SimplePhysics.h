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
    typedef ci::Vec2f Vec;
    struct Node
    {
      Vec pos;
      Vec ppos;
    };
    typedef std::shared_ptr<Node>       NodeRef;

    class Effector
    {
    public:
      //! apply effect to node
      virtual void apply( Node *node ) const = 0;
    };
    typedef std::shared_ptr<Effector>   EffectorRef;

    class Constraint
    {
    public:
      virtual void apply() const = 0;
    private:
    };
    typedef std::shared_ptr<Constraint> ConstraintRef;

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

    class World
    {
    public:
      //! advance physics one timestep
      void step( double dt );
      void drawNodes();
      //! Create a new physics node in the simulation and return it
      NodeRef createNode( const Vec &pos=Vec( 0.0f, 0.0f ) );
      //! Returns the node nearest to \a pos
      NodeRef nearestNode( const Vec &pos );

      //! Sets the global friction (1.0 being highest, 0.0 being none)
      World&  friction( float f ) { mFriction = f; return *this; }
      //! Creates a new constraint and returns a reference to the new object
      template<typename T, typename... Args>
      std::shared_ptr<T> createConstraint( Args... args );
    private:
      std::vector<NodeRef>        mNodes;
      std::vector<ConstraintRef>  mConstraints;
      std::vector<EffectorRef>    mEffectors;
      float                       mFriction = 0.5f;
      double                      mPDT = 1.0 / 60.0; // previous delta time
    };

    template<typename T, typename... Args>
    std::shared_ptr<T> World::createConstraint( Args... args )
    {
      auto constraint = std::make_shared<T>( std::forward<Args>(args)... );
      mConstraints.push_back( constraint );
      return constraint;
    }
	}
}

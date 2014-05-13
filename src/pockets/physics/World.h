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

#include "pockets/physics/Types.h"

/*
 A basic verlet physics engine.
 Useful primarily for UI animation.
*/

namespace pockets
{
	namespace physics
	{
     /**
      Container for physics simulations.
      
      step() performs Verlet integration on all nodes.
      Provides factory methods for nodes, constraints, and effectors.
     */
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
      //! Creates a new effector and returns a reference to the new object
      template<typename T, typename... Args>
      std::shared_ptr<T> createEffector( Args... args );
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

    template<typename T, typename... Args>
    std::shared_ptr<T> World::createEffector( Args... args )
    {
      auto effector = std::make_shared<T>( std::forward<Args>(args)... );
      mEffectors.push_back( effector );
      return effector;
    }
	}
}

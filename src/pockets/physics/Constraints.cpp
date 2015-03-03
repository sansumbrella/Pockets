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

#include "SimplePhysics.h"

#include "cinder/gl/gl.h"

using namespace pockets;
using namespace physics;
using namespace cinder;
using namespace std;

Range::Range( NodeRef node, Vec first, Vec second ):
mNode( node ),
mMin( std::min( first.x, second.x ), std::min( first.y, second.y ) ),
mMax( std::max( first.x, second.x ), std::max( first.y, second.y ) )
{}

void Range::apply() const
{
  auto &pos = mNode->pos;
  if( pos.x > mMax.x ){ pos.x = mMax.x; }
  else if( pos.x < mMin.x ){ pos.x = mMin.x; }
  if( pos.y > mMax.y ){ pos.y = mMax.y; }
  else if( pos.y < mMin.y ){ pos.y = mMin.y; }
}

Spring::Spring( NodeRef a, NodeRef b, float stiffness ):
mA( a ),
mB( b ),
mRestLength( distance( a->pos, b->pos ) ),
mStiffness( stiffness )
{}

void Spring::apply() const
{
  Vec delta = mB->pos - mA->pos;
  float distance = length(delta);
  float offset = mRestLength - distance;
  if( distance > ci::EPSILON_VALUE )
  { offset /= distance; }
  Vec correction = delta * offset / 2.0f * mStiffness;
  mA->pos -= correction;
  mB->pos += correction;
}

Lashing::Lashing( NodeRef pet, NodeRef owner, float stiffness ):
mPet( pet ),
mRock( owner ),
mStiffness( stiffness )
{}

void Lashing::apply() const
{
  Vec delta = mRock->pos - mPet->pos;
  mPet->pos += delta * mStiffness;
}

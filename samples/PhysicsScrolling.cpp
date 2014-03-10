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

#include "PhysicsScrolling.h"
#include "cinder/gl/gl.h"

using namespace pockets;
using namespace cinder;
using namespace cinder::app;
using namespace std;

PhysicsScrolling::PhysicsScrolling()
{}

PhysicsScrolling::~PhysicsScrolling()
{}

void PhysicsScrolling::setup()
{
  mWorld.friction( 0.5f );
  Vec2f center = getWindowCenter();
  mActualPosition = mWorld.createNode( glm::vec2( center.x, center.y ) );
  mTargetPosition = mWorld.createNode( glm::vec2( center.x, center.y ) );
  mWorld.createConstraint<physics::Lashing>( mActualPosition, mTargetPosition, 0.054f );

  mWorld.createConstraint<physics::Range>( mTargetPosition, glm::vec2( 0.0f, 0.0f ), glm::vec2( 0.0f, getWindowHeight() - 200.0f ) );
  mWorld.createConstraint<physics::Range>( mActualPosition, glm::vec2( 0.0f, -100.0f ), glm::vec2( 0.0f, getWindowHeight() - 100.0f ) );
}

void PhysicsScrolling::connect( app::WindowRef window )
{
  storeConnection( window->getSignalMouseDown().connect( [this]( const MouseEvent &e )
    { mouseDown( e ); } ) );
  storeConnection( window->getSignalMouseDrag().connect( [this]( const MouseEvent &e )
                                                        { mouseDrag( e ); } ) );
  storeConnection( window->getSignalMouseUp().connect( [this]( const MouseEvent &e )
                                                        { mouseUp( e ); } ) );
}

void PhysicsScrolling::mouseDown( MouseEvent event )
{
  mMouseDown = true;
  mMousePos.x = event.getX();
  mMousePos.y = event.getY();
  mMouseStart = mMousePos;
  mNodeStart = mTargetPosition->pos;
}

void PhysicsScrolling::mouseDrag( MouseEvent event )
{
  mMousePos.x = event.getPos().x;
  mMousePos.y = event.getPos().y;
}

void PhysicsScrolling::mouseUp( MouseEvent event )
{
  mMouseDown = false;
  mMousePos.x = event.getX();
  mMousePos.y = event.getY();
  auto pos = mNodeStart + (mMousePos - mMouseStart);
  mTargetPosition->pos = pos;
}

void PhysicsScrolling::update( double dt )
{
  if( mMouseDown )
  {
    auto pos = mNodeStart + (mMousePos - mMouseStart);
    mTargetPosition->pos = mTargetPosition->ppos = pos;
  }
  mWorld.step( 1.0 / 60.0 );
}

void PhysicsScrolling::draw() const
{
  // clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );

  gl::color( Color( 1.0f, 1.0f, 0.0f ) );
  gl::drawSolidRect( Rectf( mActualPosition->pos.x, mActualPosition->pos.y,
                            mActualPosition->pos.x + 200.0f, mActualPosition->pos.y + 200.0f ) );
  gl::color( Color( 1.0f, 0.0f, 1.0f ) );
  gl::drawSolidRect( Rectf( mTargetPosition->pos.x, mTargetPosition->pos.y,
                            mTargetPosition->pos.x + 10.0f, mTargetPosition->pos.y + 10.0f ) );
}


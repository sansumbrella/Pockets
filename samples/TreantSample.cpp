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

#include "TreantSample.h"
#include "cinder/gl/gl.h"

#include "pockets/puptent/PupTent.h"
#include "pockets/puptent/RenderSystem.h"
#include "entityx.h"
#include "entityx/System.h"
#include "cinder/Rand.h"

#include <memory>

using namespace pockets;
using namespace puptent;
using namespace cinder;
using namespace cinder::app;
using namespace std;

void TreantTest::setup()
{
  _treant.systems->add<RenderSystem>();
  _treant.systems->configure();

  _treant_root = _treant.createRoot();

  for( int i = 0; i < 10000; ++i ) {
    auto child = _treant_root->createChild<treant::TreantNode>();
    child->setPosition( randVec2f() * Vec2f( getWindowSize() ) );
    auto mesh = child->assign<RenderMesh>();
    mesh->setAsBox( Rectf( -10.0f, -10.0f, 10.0f, 10.0f ) );
    mesh->setColor( ColorA( CM_HSV, randFloat( 0.02f, 0.2f ), 1.0f, 1.0f, 1.0f ) );
    child->assign<RenderData>( mesh, child->getTransform() );
  }

  _treant_root.reset();
}

void TreantTest::connect( app::WindowRef window )
{
  storeConnection( window->getSignalMouseDown().connect( [this]( const MouseEvent &e )
    { mouseDown( e ); } ) );
  storeConnection( window->getSignalMouseDrag().connect( [this]( const MouseEvent &e )
                                                        { mouseDrag( e ); } ) );
  storeConnection( window->getSignalMouseUp().connect( [this]( const MouseEvent &e )
                                                        { mouseUp( e ); } ) );
}

void TreantTest::mouseDown( MouseEvent event )
{
  _mouse_down = true;
  _mouse_position = event.getPos();
  _mouse_start = event.getPos();
}

void TreantTest::mouseDrag( MouseEvent event )
{
  _mouse_position = event.getPos();
  _treant_root->setPosition( _mouse_position );
}

void TreantTest::mouseUp( MouseEvent event )
{
  _mouse_down = false;
  _mouse_position = event.getPos();
}

void TreantTest::update( double dt )
{
  _treant_root->updateTree( MatrixAffine2f::identity() );
  _treant.systems->update<pt::RenderSystem>( dt );
}

void TreantTest::draw()
{
  // clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );

  _treant.systems->system<RenderSystem>()->draw();

}


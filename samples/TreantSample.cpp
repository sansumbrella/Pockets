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
#include "TreantNode.h"
#include "cinder/gl/gl.h"

#include "treant/ShapeRenderSystem.h"
#include "treant/ShapeComponent.h"
#include "cinder/Rand.h"

#include <memory>

using namespace pockets;
using namespace cinder;
using namespace cinder::app;
using namespace std;

void TreantTest::setup()
{
  _treant.systems->add<treant::ShapeRenderSystem>();
  _treant.systems->configure();

  _treant_root = _treant.createRoot();

  for( int i = 0; i < 10000; ++i ) {
    auto child = _treant_root->createChild<treant::TreantNode>();
    child->setPosition( Vec2f( randFloat( -0.5f, 0.5f ), randFloat( -0.5f, 0.5f ) ) * Vec2f( getWindowSize() ) );
    auto mesh = child->assign<treant::ShapeComponent>();
    mesh->setAsBox( Rectf( -10.0f, -10.0f, 10.0f, 10.0f ) );
    mesh->setColor( ColorA( CM_HSV, randFloat( 0.02f, 0.2f ), 1.0f, 1.0f, 1.0f ) );
    child->assign<treant::RenderData>( mesh, child->getTransform() );
  }
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
  _node_start = _treant_root->getPosition();
}

void TreantTest::mouseDrag( MouseEvent event )
{
  _mouse_position = event.getPos();
}

void TreantTest::mouseUp( MouseEvent event )
{
  _mouse_down = false;
  _mouse_position = event.getPos();
}

void TreantTest::update( double dt )
{
  auto delta = _mouse_position - _mouse_start;
  _treant_root->setPosition( _node_start + delta );

  _treant_root->updateTree( MatrixAffine2f::identity() );
  _treant.systems->update<treant::ShapeRenderSystem>( dt );
}

void TreantTest::draw()
{
  // clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );

  _treant.systems->system<treant::ShapeRenderSystem>()->draw();

}


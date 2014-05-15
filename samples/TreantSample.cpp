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

#include "treant/LayeredShapeRenderSystem.h"
#include "treant/TextRenderSystem.h"
#include "treant/ShapeComponent.h"
#include "cinder/Rand.h"

#include <memory>

#define USE_SMOOTHED_ROTATION 1

using namespace pockets;
using namespace cinder;
using namespace cinder::app;
using namespace std;

struct RotationComponent : treant::Component<RotationComponent>
{
  RotationComponent() = default;
  RotationComponent( float rate ):
    rate( rate )
  {}

  float  rate = 1.0f;
};

class RotationSystem : public treant::System<RotationSystem>
{
public:
  void update( treant::EntityManagerRef entities, treant::EventManagerRef events, double dt ) override
  {
    for( auto entity : entities->entities_with_components<RotationComponent, treant::LocationComponent>() )
    {
      treant::LocationComponentRef  location;
      shared_ptr<RotationComponent> rotation;
      entity.unpack( location, rotation );
#if USE_SMOOTHED_ROTATION
      // lerp(curr, target, 1.0-pow(1.0-rate, targetFPS*dt)
      // smoothed step
      float target = location->rotation + rotation->rate / 60.0;
      location->rotation = lerp( location->rotation, target, 1.0 - pow(0.01, 60.0 * dt) );
#else
      location->rotation += rotation->rate * dt;
#endif
    }
  }
};

treant::TreantNodeRef addOrbiter( treant::TreantNodeRef center, bool warm, float max_distance, int depth ) {

  float size = center->getSize().length() / 5.0f;
  auto moon = center->createChild<treant::TreantNode>();
  moon->setSize( Vec2f::one() * size );

  Vec2f position = randVec2f() * randFloat( max_distance / 4, max_distance );
  moon->setPosition( position );
  moon->setRegistrationPoint( -position );
  auto shape = moon->assign<treant::ShapeComponent>();
  auto color = warm ? ColorA( CM_HSV, randFloat( 0.02f, 0.18f ), 1.0f, 1.0f, 1.0f ) : ColorA( CM_HSV, randFloat( 0.45f, 0.62f ), 1.0f, 0.7f, 1.0f );

  shape->setAsBox( Rectf( -size, -size, size, size ) );
  shape->setColor( color );
  moon->assign<treant::LayeredShapeRenderData>( shape, moon->getTransform(), depth );
  moon->assign<RotationComponent>( lmap<float>( position.length(), 0.0f, getWindowSize().length(), 1.0f, 0.1f ) );

  if( size > 10.0f && randFloat() < 0.5f ) {
    addOrbiter( moon, !warm, max_distance / 8, depth + 1 );
  }

  return moon;
}

void TreantTest::setup()
{
  _treant.systems->add<treant::LayeredShapeRenderSystem>();
  _treant.systems->add<treant::TextRenderSystem>();
  _treant.systems->add<RotationSystem>();
  _treant.systems->configure();

  _treant_root = _treant.createRoot();
  _treant_root->setPosition( getWindowCenter() );
  _treant_root->setSize( getWindowSize() / 4 );

  Font arial( "Arial Bold", 24.0f );
  gl::TextureFontRef font = gl::TextureFont::create( arial, gl::TextureFont::Format().premultiply() );

  for( int i = 0; i < 1000; ++i )
  {
    auto moon = addOrbiter( _treant_root, true, getWindowSize().length(), 0 );
    if( randFloat() < 0.05f ) {
      moon->assign<treant::TextComponent>( font, "I am planet " + to_string( i ) );
    }
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

  _treant.systems->update<RotationSystem>( dt );
  _treant_root->updateTree( MatrixAffine2f::identity() );
  _treant.systems->update<treant::LayeredShapeRenderSystem>( dt );
  _treant.systems->update<treant::TextRenderSystem>( dt );
}

void TreantTest::draw()
{
  // clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );

  _treant.systems->system<treant::LayeredShapeRenderSystem>()->draw();

  gl::ScopedAlphaBlend premult( true );
  _treant.systems->system<treant::TextRenderSystem>()->draw();

}


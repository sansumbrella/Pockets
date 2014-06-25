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

#include "TreentSample.h"
#include "TreentNode.h"
#include "cinder/gl/gl.h"

#include "treent/LayeredShapeRenderSystem.h"
#include "treent/TextRenderSystem.h"
#include "treent/ShapeComponent.h"
#include "cinder/Rand.h"

#include <memory>

#define USE_SMOOTHED_ROTATION 1

using namespace pockets;
using namespace cinder;
using namespace cinder::app;
using namespace std;

namespace
{

struct RotationComponent : treent::Component<RotationComponent>
{
  RotationComponent() = default;
  RotationComponent( float rate ):
  rate( rate )
  {}

  float  rate = 1.0f;
};

class RotationSystem : public treent::System<RotationSystem>
{
public:
  void update( treent::EntityManagerRef entities, treent::EventManagerRef events, double dt ) override
  {
    for( auto entity : entities->entities_with_components<RotationComponent, treent::LocationComponent>() )
    {
      treent::LocationComponentRef  location;
      shared_ptr<RotationComponent> rotation;
      entity.unpack( location, rotation );
#if USE_SMOOTHED_ROTATION
      // lerp(curr, target, 1.0-pow(1.0-rate, targetFPS*dt)
      // smoothed step
      float target = location->rotation + rotation->rate / 60.0;
      location->rotation = lerp<float>( location->rotation, target, 1.0 - pow(0.01, 60.0 * dt) );
#else
      location->rotation += rotation->rate * dt;
#endif
    }
  }
};

treent::TreentNodeRef addOrbiter( treent::TreentNodeRef center, bool warm, float max_distance, int depth ) {

  float size = center->getSize().length() / 5.0f;
  auto moon = center->createChild<treent::TreentNode>();
  moon->setSize( Vec2f::one() * size );

  Vec2f position = randVec2f() * randFloat( max_distance / 4, max_distance );
  moon->setPosition( position );
  moon->setRegistrationPoint( -position );
  auto shape = moon->assign<treent::ShapeComponent>();
  auto color = warm ? ColorA( CM_HSV, randFloat( 0.02f, 0.18f ), 1.0f, 1.0f, 1.0f ) : ColorA( CM_HSV, randFloat( 0.45f, 0.62f ), 1.0f, 0.7f, 1.0f );

  shape->setAsBox( Rectf( -size, -size, size, size ) );
  shape->setColor( color );
  moon->assign<treent::LayeredShapeRenderData>( shape, moon->getTransform(), depth );
  moon->assign<RotationComponent>( lmap<float>( position.length(), 0.0f, getWindowSize().length(), 1.0f, 0.1f ) );

  if( size > 10.0f && randFloat() < 0.5f ) {
    addOrbiter( moon, !warm, max_distance / 8, depth + 1 );
  }
  
  return moon;
}
  
} // anon::

void TreentTest::setup()
{
  _treent.systems->add<treent::LayeredShapeRenderSystem>();
  _treent.systems->add<treent::TextRenderSystem>();
  _treent.systems->add<RotationSystem>();
  _treent.systems->configure();

  _treent_root = _treent.createRoot();
  _treent_root->setPosition( getWindowCenter() );
  _treent_root->setSize( getWindowSize() / 4 );
  auto shape = _treent_root->assign<treent::ShapeComponent>();
  shape->setAsCircle( Vec2f::one() * getWindowWidth() / 6, 0, M_PI * 2, 6 );  // hexagon
  _treent_root->assign<treent::LayeredShapeRenderData>( shape, _treent_root->getTransform(), 0 );

  Font arial( "Arial Bold", 24.0f );
  gl::TextureFontRef font = gl::TextureFont::create( arial, gl::TextureFont::Format().premultiply() );

  for( int i = 0; i < 1000; ++i )
  {
    auto moon = addOrbiter( _treent_root, true, getWindowSize().length(), 0 );
    if( randFloat() < 0.05f ) {
      moon->assign<treent::TextComponent>( font, "I am planet " + to_string( i ) );
    }
  }
}

void TreentTest::connect( app::WindowRef window )
{
  storeConnection( window->getSignalMouseDown().connect( [this]( const MouseEvent &e )
    { mouseDown( e ); } ) );
  storeConnection( window->getSignalMouseDrag().connect( [this]( const MouseEvent &e )
                                                        { mouseDrag( e ); } ) );
  storeConnection( window->getSignalMouseUp().connect( [this]( const MouseEvent &e )
                                                        { mouseUp( e ); } ) );
}

void TreentTest::mouseDown( MouseEvent event )
{
  _mouse_down = true;
  _mouse_position = event.getPos();
  _mouse_start = event.getPos();
  _node_start = _treent_root->getPosition();
}

void TreentTest::mouseDrag( MouseEvent event )
{
  _mouse_position = event.getPos();
}

void TreentTest::mouseUp( MouseEvent event )
{
  _mouse_down = false;
  _mouse_position = event.getPos();
}

void TreentTest::update( double dt )
{
  auto delta = _mouse_position - _mouse_start;
  _treent_root->setPosition( _node_start + delta );

  _treent.systems->update<RotationSystem>( dt );
  _treent_root->updateTree( MatrixAffine2f::identity() );
  _treent.systems->update<treent::LayeredShapeRenderSystem>( dt );
  _treent.systems->update<treent::TextRenderSystem>( dt );
}

void TreentTest::draw()
{
  // clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );

  _treent.systems->system<treent::LayeredShapeRenderSystem>()->draw();

  gl::ScopedAlphaBlend premult( true );
  _treent.systems->system<treent::TextRenderSystem>()->draw();

}


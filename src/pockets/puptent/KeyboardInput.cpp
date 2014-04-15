//
//  Copyright (c) 2013 David Wicks, sansumbrella.com.
//  All rights reserved.
//

#include "puptent/KeyboardInput.h"
#include "pockets/CollectionUtilities.hpp"
#include "cinder/app/App.h"

using namespace std;
using namespace cinder;
using namespace cinder::app;
using namespace pockets::puptent;

KeyboardInput::KeyboardInput()
{}

KeyboardInput::~KeyboardInput()
{}

KeyboardInputRef KeyboardInput::create()
{
  return KeyboardInputRef{ new KeyboardInput };
}

void KeyboardInput::connect( ci::app::WindowRef window )
{
  mConnections.store( window->getSignalKeyDown().connect(
  [this]( const KeyEvent &event ) {
    mDownEvents.emplace_back( event );
  } ) );
  mConnections.store( window->getSignalKeyUp().connect(
  [this]( const KeyEvent &event ) {
    mUpEvents.emplace_back( event );
  } ) );
  // connect update at front to guarantee most recent events are used in scripts
  mConnections.store( App::get()->getSignalUpdate().connect(
  [this]() {
    update();
  },
  signals::at_front ) );
}

void KeyboardInput::update()
{
  mPressedKeys.clear();
  mReleasedKeys.clear();
  for( const auto &event : mDownEvents )
  {
    keyDown( event );
  }
  for( const auto &event : mUpEvents )
  {
    keyUp( event );
  }
  mUpEvents.clear();
  mDownEvents.clear();
  // update directional forces
  mForce.set( 0.0f, 0.0f );
  for( const auto key : mHeldKeys )
  {
    switch ( key )
    {
      case KeyEvent::KEY_LEFT:
        mForce.x = -1.0f;
        break;
      case KeyEvent::KEY_RIGHT:
        mForce.x = 1.0f;
        break;
      case KeyEvent::KEY_DOWN:
        mForce.y = 1.0f;
        break;
      case KeyEvent::KEY_UP:
        mForce.y = -1.0f;
        break;
      default:
        break;
    }
  }
}

void KeyboardInput::keyDown( const KeyEvent &event )
{
  mPressedKeys.insert( event.getCode() );
  mHeldKeys.push_back( event.getCode() );
}

void KeyboardInput::keyUp( const ci::app::KeyEvent &event )
{
  mReleasedKeys.insert( event.getCode() );
  pk::vector_remove( &mHeldKeys, event.getCode() );
}

bool KeyboardInput::isKeyDown(int key) const
{
  return pk::vector_contains( mHeldKeys, key );
}

bool KeyboardInput::wasKeyPressed(int key) const
{
  return mPressedKeys.find( key ) != mPressedKeys.end();
}

bool KeyboardInput::wasKeyReleased( int key ) const
{
  return mReleasedKeys.find( key ) != mReleasedKeys.end();
}

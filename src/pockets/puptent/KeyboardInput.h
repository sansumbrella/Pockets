//
//  Copyright (c) 2013 David Wicks, sansumbrella.com.
//  All rights reserved.
//

#pragma once
#include "pockets/ConnectionManager.h"
#include "cinder/app/Window.h"
#include <set>

/**
 KeyboardInput:
 Receives user KeyboardInput events and stores them in a format
 useful for the main game update.
 Arrow keys get stored as normalized xy forces.
 TODO: mapping from keycodes to "buttons" for easier script access
 Note that hardware key rollover limits can prevent high-quality KeyboardInput.
 e.g. on my keyboard the horizontal direction keys don't register if both
 vertical direction keys are held.
*/

namespace pockets
{ namespace puptent
{
  typedef std::shared_ptr<class KeyboardInput> KeyboardInputRef;
  class KeyboardInput
  {
  public:
    KeyboardInput();
    ~KeyboardInput();
    //! connect to keyboard events for given window and start updating
    void connect( ci::app::WindowRef window );
    //! stop receiving input (also stops reporting keys and force)
    void pause(){ mConnections.block(); mHeldKeys.clear(); }
    //! resume receiving input
    void resume(){ mConnections.resume(); }

    //! returns normalized force along xy axes
    auto getForce() -> ci::Vec2f const { return mForce; }
    //! returns true if the key with code key is down
    bool getKeyDown( int key ) const;
    //! returns true if the key with code key was pressed this frame
    bool getKeyPressed( int key ) const;
    //! returns true if the key with code key was released this frame
    bool getKeyReleased( int key ) const;
    //! creates a new KeyboardInputRef
    static auto create() -> KeyboardInputRef;
  private:
    pk::ConnectionManager           mConnections;
    ci::Vec2f                       mForce = ci::Vec2f::zero();
    std::vector<int>                mHeldKeys;
    std::set<int>                   mPressedKeys;
    std::set<int>                   mReleasedKeys;
    std::vector<ci::app::KeyEvent>  mDownEvents;
    std::vector<ci::app::KeyEvent>  mUpEvents;

    void keyDown( const ci::app::KeyEvent &event );
    void keyUp( const ci::app::KeyEvent &event );
    void update();
  };
} // puptent::
} // pockets::

//
//  SimpleButton.h
//  WordShift
//
//  Created by David Wicks on 2/22/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/gl/Texture.h"
#include "Node.h"
#include "ButtonBase.h"

/**
A generic interactive interface is as simple as
virtual void connect( ci::app::WindowRef window );
void disconnect(); // disconnects all stored connections
storeConnection( connection ); // hold onto connection for mgmt
//
*/

namespace cascade
{
  typedef std::shared_ptr<class SimpleButton> SimpleButtonRef;
  /**
  A simple button specialized for Word Shift's needs
  Displays a texture on top of a rectangular backing
  Pulls color from the global game color functions based on state.
  TODO: make a button that can display sprited graphics. Or an intermediary
  ImageType that can wrap either a Sprited graphic or a Texture.
  */
  class SimpleButton : public ButtonBase
  {
  public:
    ~SimpleButton();
    void            draw();
    void            setHitPadding( float horizontal, float vertical );
    //! create a button with a string label foreground set in \a font
    static SimpleButtonRef createLabelButton( const std::string &str, const ci::Font &font );
    //! create a button from a foreground graphic centered in bounds
    static SimpleButtonRef create( const ci::gl::Texture &foreground, const ci::Rectf &bounds );
    void            hoverStart();
    void            hoverEnd();
  private:
    SimpleButton( const ci::gl::Texture &foreground, const ci::Rectf &bounds );
    //! screen boundary of element
    ci::Rectf           mBackgroundBounds;
    ci::Rectf           mForegroundBounds;

    ci::gl::Texture     mForegroundTexture;
    ci::Color           mBackingColor = getBackingColor();
    ci::Color           mForegroundColor = getTextColor();
  };

} // cascade::

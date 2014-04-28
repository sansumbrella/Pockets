//
//  SpriteToggle.cpp
//  WordShift
//
//  Created by David Wicks on 5/15/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "SpriteToggle.h"

using namespace pockets;
using namespace ci;
using namespace std;


SpriteToggle::SpriteToggle( const Sprite &sprite_true, const Sprite &sprite_false, const GetterFn &getter, const SetterFn &setter ):
ButtonBase( sprite_true.getLocalBounds() )
, mBackingColor( getBackingColor() )
, mIconColor( getTextColor() )
, mSpriteTrue( sprite_true )
, mSpriteFalse( sprite_false )
, mGetter( getter )
, mSetter( setter )
{
  setSelectFn( [this](){ toggle(); } );
  mSprite = mGetter() ? &mSpriteTrue : &mSpriteFalse;
}

SpriteToggleRef SpriteToggle::create(const pk::Sprite &sprite_true, const pk::Sprite &sprite_false, const GetterFn &getter, const SetterFn &setter)
{
  return SpriteToggleRef( new SpriteToggle( sprite_true, sprite_false, getter, setter ) );
}

SpriteToggle::~SpriteToggle()
{}

void SpriteToggle::updateColor()
{
  mBackingColor = getBackingColor();
  mIconColor = getTextColor();
}

void SpriteToggle::toggle()
{
  bool value = !mGetter();
  mSetter( value );
  mSprite = value ? &mSpriteTrue : &mSpriteFalse;
}

void SpriteToggle::hoverStart()
{
  mBackingColor = getBackingHighlightColor();
}

void SpriteToggle::hoverEnd()
{
  mBackingColor = getBackingColor();
}

void SpriteToggle::draw()
{
  if( isActive() )
  {
    gl::disable( GL_TEXTURE_2D );
    /*  Draw the hit bounds
     gl::color( ColorA( 1.0f, 0.0f, 0.0f, 0.5f ) );
     gl::drawSolidRect( getHitBounds() );
     // */
    gl::color( mBackingColor );
    gl::drawSolidRect( mSprite->getLocalBounds() );
    gl::enable( GL_TEXTURE_2D );
    gl::color( mIconColor );
    mSprite->draw();
  }
  else
  { // blend into the background
    gl::color( ColorA( getAltTextColor(), 0.4f ) );
    gl::disable( GL_TEXTURE_2D );
    gl::drawSolidRect( mSprite->getLocalBounds() );
    gl::enable( GL_TEXTURE_2D );
    gl::color( getBackgroundColor() );
    mSprite->draw();
  }
}

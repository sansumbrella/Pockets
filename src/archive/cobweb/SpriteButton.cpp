//
//  SpriteButton.cpp
//  WordShift
//
//  Created by David Wicks on 4/24/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "SpriteButton.h"

using namespace cinder;
using namespace pockets;

SpriteButton::SpriteButton( const Sprite &icon ):
ButtonBase( icon.getLocalBounds() )
, mIcon( icon )
, mBackingColor( getBackingColor() )
, mIconColor( getTextColor() )
{}

SpriteButton::~SpriteButton()
{}

void SpriteButton::updateColor()
{
  mBackingColor = getBackingColor();
  mIconColor = getTextColor();
}

void SpriteButton::hoverStart()
{
  mBackingColor = getBackingHighlightColor();
}

void SpriteButton::hoverEnd()
{
  mBackingColor = getBackingColor();
}

void SpriteButton::draw()
{
  if( isActive() )
  {
    gl::disable( GL_TEXTURE_2D );
    /*  Draw the hit bounds
     gl::color( ColorA( 1.0f, 0.0f, 0.0f, 0.5f ) );
     gl::drawSolidRect( getHitBounds() );
     */
    gl::color( mBackingColor );
    gl::drawSolidRect( mIcon.getLocalBounds() );
    gl::enable( GL_TEXTURE_2D );
    gl::color( mIconColor );
    mIcon.draw();
  }
  else
  { // blend into the background
    gl::color( ColorA( getAltTextColor(), 0.4f ) );
    gl::disable( GL_TEXTURE_2D );
    gl::drawSolidRect( mIcon.getLocalBounds() );
    gl::enable( GL_TEXTURE_2D );
    gl::color( getBackgroundColor() );
    mIcon.draw();
  }
}

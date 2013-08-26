//
//  SimpleButton.cpp
//  WordShift
//
//  Created by David Wicks on 2/22/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "SimpleButton.h"
#include "cinder/Text.h"

using namespace std;
using namespace ci;
using namespace pockets;
using namespace cascade;

SimpleButton::SimpleButton( const gl::Texture &fg, const ci::Rectf &bounds ):
ButtonBase( Area(bounds) )
, mForegroundTexture( fg )
, mBackgroundBounds( bounds )
, mForegroundBounds( app::toPoints(Rectf(fg.getBounds())).getCenteredFit( mBackgroundBounds, false ) )
{
  getLocus()->setRegistrationPoint( mBackgroundBounds.getCenter() );
}

SimpleButton::~SimpleButton()
{}

SimpleButtonRef SimpleButton::create(const ci::gl::Texture &foreground, const ci::Rectf &bounds )
{
  return SimpleButtonRef( new SimpleButton( foreground, bounds ) );
}

SimpleButtonRef SimpleButton::createLabelButton(const std::string &str, const ci::Font &font)
{
  TextLayout layout;
  layout.clear( ColorA( 0, 0, 0, 0.0f ) );
  layout.setColor( Color::white() );
  layout.setFont( font );
  layout.setBorder( font.getSize() / 8, font.getSize() / 8 );
  layout.addLine( str );
  Surface fg = layout.render( true, true );
  Rectf bounds( 0, 0, app::toPoints(fg.getWidth() + font.getSize() * 0.5f), app::toPoints(fg.getHeight() + font.getSize() * 0.375f) );

  return SimpleButton::create( fg, bounds );
}

void SimpleButton::setHitPadding(float horizontal, float vertical)
{
  setHitBounds( mBackgroundBounds );
  expandHitBounds( horizontal, vertical );
}

void SimpleButton::hoverStart()
{
  mBackingColor = getBackingHighlightColor();
}

void SimpleButton::hoverEnd()
{
  mBackingColor = getBackingColor();
}

void SimpleButton::draw()
{
  gl::pushModelView();
  gl::multModelView( *getLocus() );

  //  gl::color( Color( 1, 0, 0 ) );
  //  gl::drawSolidRect( getHitBounds() );

  gl::color( mBackingColor );
  gl::drawSolidRect( mBackgroundBounds );

  mForegroundTexture.enableAndBind();
  gl::color( mForegroundColor );
  gl::drawSolidRect( mForegroundBounds );
  mForegroundTexture.unbind();

  gl::popModelView();
}

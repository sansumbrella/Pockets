//
//  TextPane.cpp
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "TextPane.h"
#include "cinder/ip/Fill.h"
#include "cinder/ip/Resize.h"
#include "cinder/Text.h"

using namespace pockets;
using namespace cinder;

TextPane::TextPane( const Font &font, int width )
{
  setWidth( width );

  TextLayout layout;
  layout.setFont( font );
  layout.setColor( Color::white() );
  layout.addLine( "0" );
  Surface render = layout.render( true, true );
  const int height = app::toPoints( render.getHeight() );
  setHeight( height );

  mSurface = Surface( app::toPixels( width ), app::toPixels( height ), true, SurfaceChannelOrder::BGRA );
  ip::fill( &mSurface, ColorA( 0, 0, 0, 0 ) );

  mScreenBounds = { 0, 0, width, height };
  mTextureRegion = app::toPixels( Area(0, 0, width, height) );

  mTexture = gl::Texture( mSurface );
}

void TextPane::setTextSoft( const std::string &text, float delay )
{
  // slide down, change when hidden
  mTimeline->appendTo( &mTextureOffset, Vec2f( 0, app::toPixels(-getHeight()) ), 0.33f, EaseInQuint() )
  .delay( delay )
  .finishFn( [=](){ setText( text ); } );
  // slide up
  mTimeline->appendTo( &mTextureOffset, Vec2f::zero(), 0.4f, EaseInOutQuint() );
}

void TextPane::setText(const std::string &text)
{
  TextLayout layout;
//  layout.setFont( getButtonFont() );
  layout.clear( ColorA( 0, 0, 0, 0 ) );
  layout.setColor( Color::white() );
  layout.addLine( text );
  Surface render = layout.render( true, true );
  ip::fill( &mSurface, ColorA( 0, 0, 0, 0 ) );
  if( mCentered )
  {
    mSurface.copyFrom( render, render.getBounds(), Vec2i( (mSurface.getWidth() - render.getWidth())/2, 0 ) );
  }
  else
  {
    mSurface.copyFrom( render, render.getBounds() );
  }
  mTexture.update( mSurface );
}

void TextPane::update( float deltaTime )
{
  mTimeline->step( deltaTime );
}

void TextPane::draw()
{
  gl::pushModelView();
  gl::multModelView( *getLocus() );
  gl::draw( mTexture, mTextureRegion + mTextureOffset(), mScreenBounds );
  gl::popModelView();
}


//
//  ColorThemeButton.cpp
//  WordShift
//
//  Created by David Wicks on 4/28/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "ColorThemeButton.h"
#include <boost/algorithm/string/case_conv.hpp>

using namespace cascade;
using namespace pockets;
using namespace cinder;
using namespace std;

ColorThemeButton::ColorThemeButton( float width, const GamePalette &palette, const pk::SpriteSheet &sprite_sheet ):
ButtonBase( Rectf( 0, 0, width, 0 ) )
, mBackgroundColor( palette.getBackground() )
{
  string name = palette.getName();
  Sprite backing = sprite_sheet.getSprite( "tile-backing" );
  Sprite corner_tl = sprite_sheet.getSprite( "corner" );
  Sprite letter = sprite_sheet.getSprite( boost::algorithm::to_upper_copy( name.substr( 0, 1 ) ) );
  Sprite label = sprite_sheet.getSprite( "cp-" + name + "-label" );

  backing.setTint( palette.getBacking() );
  corner_tl.setTint( palette.getBackingHighlight() );
  letter.setTint( palette.getText() );
  label.setTint( palette.getAltText() );

  Sprite corner_tr = corner_tl;
  Sprite corner_br = corner_tl;
  Sprite corner_bl = corner_tl;

  const int edging = sprite_sheet.getSpriteData("line-arrow").getSize().y;
  setWidth( width );
  setHeight( backing.getSize().y + edging * 2 );

  // rotate since base graphic is for bottom-right corner
  corner_tl.getLocus().setRotation( M_PI );
  corner_tr.getLocus().setRotation( -M_PI / 2 );
  corner_bl.getLocus().setRotation( M_PI / 2 );

  backing.setLoc( backing.getRegistrationPoint() + Vec2i( edging, edging ) );
  corner_tl.setLoc( backing.getLoc() - backing.getSize() / 2 );
  corner_br.setLoc( backing.getLoc() + backing.getSize() / 2 );
  corner_tr.setLoc( backing.getLoc() + Vec2f(backing.getSize()) * Vec2f( 0.5f, -0.5f ) );
  corner_bl.setLoc( backing.getLoc() + Vec2f(backing.getSize()) * Vec2f( -0.5f, 0.5f ) );
  letter.setLoc( backing.getLoc() );
  label.setLoc( Vec2i( backing.getSize().x + edging * 4, getHeight() * 0.66f  ) );

  mSprites.push_back( backing );
  mSprites.push_back( corner_tl );
  mSprites.push_back( corner_tr );
  mSprites.push_back( corner_br );
  mSprites.push_back( corner_bl );
  mSprites.push_back( letter );
  mSprites.push_back( label );

  setHitBounds( Rectf( 0, 0, getWidth(), getHeight() ) );
}

ColorThemeButton::~ColorThemeButton()
{}

void ColorThemeButton::draw()
{
  gl::disable( GL_TEXTURE_2D );
  gl::color( mBackgroundColor );
  gl::pushModelView();
  gl::multModelView( *getLocus() );
  gl::drawSolidRect( Rectf( Vec2f::zero(), getSize() ) );
  gl::enable( GL_TEXTURE_2D );
  for( auto &sprite : mSprites )
  {
    sprite.render();
  }
  gl::popModelView();
}


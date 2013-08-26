//
//  TextureFontNode.cpp
//  WordShift
//
//  Created by David Wicks on 5/31/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "TextureFontNode.h"

using namespace cascade;
using namespace cinder;
using namespace std;

TextureFontNode::TextureFontNode( gl::TextureFontRef font, const string &text ):
Node( font->measureString( text ) / app::getWindow()->getContentScale() )
, mFont( font )
, mText( text )
{}

TextureFontNode::~TextureFontNode()
{}

void TextureFontNode::draw()
{
  gl::pushModelView();
  gl::multModelView( *getLocus() );

  gl::TextureFont::DrawOptions opt;
  opt.scale( 1.0f / app::getWindow()->getContentScale() );
  opt.pixelSnap( false );
  mFont->drawString( mText, Vec2f::zero(), opt );

  gl::popModelView();
}

unique_ptr<TextureFontNode> TextureFontNode::create(ci::gl::TextureFontRef font, const std::string &text)
{
  return unique_ptr<TextureFontNode>( new TextureFontNode( font, text ) );
}


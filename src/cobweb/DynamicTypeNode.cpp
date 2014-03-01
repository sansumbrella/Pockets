//
//  DynamicTypeNode.cpp
//  WordShift
//
//  Created by David Wicks on 6/15/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "DynamicTypeNode.h"
#include "cinder/gl/TextureFont.h"

using namespace pockets;
using namespace cinder;
using namespace std;

DynamicTypeNode::DynamicTypeNode( gl::TextureFontRef font, const std::string &text ):
Node( font->measureString( text ) / app::getWindow()->getContentScale() ),
mFont( font )
{
  gl::TextureFont::DrawOptions opt;
  opt.scale( 1.0f / app::getWindow()->getContentScale() ).pixelSnap( false );
  mGlyphs = font->getGlyphPlacements( text, opt );
}

DynamicTypeNode::~DynamicTypeNode()
{}

void DynamicTypeNode::draw()
{
  gl::pushModelView();
  gl::multModelView( *getLocus() );

  mFont->drawGlyphs( mGlyphs, Vec2f::zero() );

  gl::popModelView();
}

DynamicTypeNodeUniqueRef DynamicTypeNode::create( gl::TextureFontRef font, const std::string &text )
{
  return DynamicTypeNodeUniqueRef{ new DynamicTypeNode{ font, text } };
}

//
//  DynamicTypeNode.cpp
//  WordShift
//
//  Created by David Wicks on 6/15/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "DynamicTypeNode.h"

using namespace cascade;
using namespace cinder;
using namespace std;

DynamicTypeNode::DynamicTypeNode( FontType type, const std::string &text ):
Node( getTextureFont(type).measureString( text ) / app::getWindow()->getContentScale() ),
mType( type ),
mText( text )
{}

DynamicTypeNode::~DynamicTypeNode()
{}

void DynamicTypeNode::draw()
{
  gl::pushModelView();
  gl::multModelView( *getLocus() );

  gl::TextureFont::DrawOptions opt;
  opt.scale( 1.0f / app::getWindow()->getContentScale() );
  opt.pixelSnap( false );
  getTextureFont(mType).drawString( mText, Vec2f::zero(), opt );

  gl::popModelView();
}

unique_ptr<DynamicTypeNode> DynamicTypeNode::create(cascade::FontType type, const std::string &text)
{
  return unique_ptr<DynamicTypeNode>( new DynamicTypeNode(type, text) );
}

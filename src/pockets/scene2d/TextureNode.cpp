//
//  TextureNode.cpp
//  WordShift
//
//  Created by David Wicks on 5/3/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "TextureNode.h"

using namespace pockets;
using namespace cinder;

TextureNode::TextureNode( const gl::Texture &texture ):
Node( app::toPoints( texture.getSize() ) )
, mTexture( texture )
{}

TextureNode::~TextureNode()
{}

void TextureNode::draw()
{
  gl::pushModelView();
  gl::multModelView( *getLocus() );

  gl::draw( mTexture, app::toPoints( mTexture.getBounds() ) );

  gl::popModelView();
}

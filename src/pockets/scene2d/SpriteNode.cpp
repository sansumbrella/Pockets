//
//  SpriteNode.cpp
//  WordShift
//
//  Created by David Wicks on 5/3/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "SpriteNode.h"

using namespace cascade;

SpriteNode::SpriteNode( const pk::Sprite &sprite ):
Node( sprite.getSize() )
,mSprite( sprite )
{
  mSprite.getLocus().setParent( getLocus() );
}

SpriteNode::~SpriteNode()
{}

void SpriteNode::draw()
{
  mSprite.render();
}


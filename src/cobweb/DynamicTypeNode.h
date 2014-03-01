//
//  DynamicTypeNode.h
//  WordShift
//
//  Created by David Wicks on 6/15/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "Node.h"

namespace cinder
{
  namespace gl
  {
    // forward decl
    typedef std::shared_ptr<class TextureFont> TextureFontRef;
  }
}

namespace pockets
{
  typedef std::shared_ptr<class DynamicTypeNode> DynamicTypeNodeRef;
  typedef std::unique_ptr<class DynamicTypeNode> DynamicTypeNodeUniqueRef;
  class DynamicTypeNode : public Node
  {
  public:
    DynamicTypeNode( ci::gl::TextureFontRef font, const std::string &text );
    ~DynamicTypeNode();
    static DynamicTypeNodeUniqueRef create( ci::gl::TextureFontRef font, const std::string &text);
    void draw();
  private:
    ci::gl::TextureFontRef  mFont;
    std::vector< std::pair< uint16_t, ci::Vec2f > > mGlyphs;
  };
}

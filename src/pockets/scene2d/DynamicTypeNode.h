//
//  DynamicTypeNode.h
//  WordShift
//
//  Created by David Wicks on 6/15/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "Node.h"

namespace cascade
{
  class DynamicTypeNode : public Node
  {
  public:
    DynamicTypeNode( FontType type, const std::string &text );
    ~DynamicTypeNode();
    static std::unique_ptr<DynamicTypeNode> create(FontType type, const std::string &text);
    void draw();
  private:
    FontType      mType;
    std::string   mText;
  };
}

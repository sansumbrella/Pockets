//
//  SpriteButton.h
//  WordShift
//
//  Created by David Wicks on 4/24/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ButtonBase.h"
#include "pockets/Sprite.h"

/**
 A button displayed as a pair of sprites (backing and foreground)
*/
namespace cascade
{
  typedef std::shared_ptr<class SpriteButton> SpriteButtonRef;
  class SpriteButton : public ButtonBase
  {
  public:
    SpriteButton( const pk::Sprite &icon );
    ~SpriteButton();

    void draw();
    void hoverStart();
    void hoverEnd();
    void updateColor();
  private:
    pk::Sprite  mIcon;
    ci::Color   mBackingColor;
    ci::Color   mIconColor;
  };
}

//
//  ColorThemeButton.h
//  WordShift
//
//  Created by David Wicks on 4/28/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ButtonBase.h"
#include "pockets/SpriteSheet.h"
#include "GameColor.h"

namespace cascade
{
  class ColorThemeButton : public ButtonBase
  {
  public:
    ColorThemeButton( float width, const GamePalette &palette, const pk::SpriteSheet &sheet );
    ~ColorThemeButton();
    void draw();
  private:
    // backing, letter, corners
    std::vector<pk::Sprite>   mSprites;
    ci::Color                 mBackgroundColor;
  };
}

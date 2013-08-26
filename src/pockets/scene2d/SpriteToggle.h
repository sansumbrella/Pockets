//
//  SpriteToggle.h
//  WordShift
//
//  Created by David Wicks on 5/15/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ButtonBase.h"
#include "Sprite.h"

namespace cascade
{
  typedef std::shared_ptr<class SpriteToggle> SpriteToggleRef;
  class SpriteToggle : public ButtonBase
  {
  public:
    typedef std::function<bool ()>      GetterFn;
    typedef std::function<void (bool)>  SetterFn;
    ~SpriteToggle();
    void  toggle();
    void  updateColor();
    void  hoverStart();
    void  hoverEnd();
    void  draw();
    static SpriteToggleRef create( const pk::Sprite &sprite_true, const pk::Sprite &sprite_false, const GetterFn &getter, const SetterFn &setter );
  private:
    // since we reference 'this' in the selectFn, we don't want it to change
    SpriteToggle( const pk::Sprite &sprite_true, const pk::Sprite &sprite_false, const GetterFn &getter, const SetterFn &setter );
    GetterFn    mGetter;
    SetterFn    mSetter;
    pk::Sprite  *mSprite = nullptr;
    pk::Sprite  mSpriteTrue;
    pk::Sprite  mSpriteFalse;
    ci::Color   mBackingColor;
    ci::Color   mIconColor;
  };
}

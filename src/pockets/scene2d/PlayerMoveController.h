//
//  PlayerMoveController.h
//  WordShift
//
//  Created by David Wicks on 5/13/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "LetterModel.h"
#include "SimpleRenderer.h"
#include "Sprite.h"

namespace cascade
{
  class PlayerMoveController : public pk::SimpleRenderer::IRenderable
  {
  public:
    PlayerMoveController( const pk::Sprite &horizontal, const pk::Sprite &vertical );
    ~PlayerMoveController();
    //! draw dots at each potential swap location
    void render();
    //! for updating in tutorial, etc, assumes no turn/play restriction
    void updateWithLetter( uint32_t touch, LetterRef letter, bool canUseLetter=true );
    void apply( uint32_t touch, LetterRef letter=nullptr, bool canUseLetter=true );
    void setCellSize( const ci::Vec2i &size ){ mCellSize = size; }
    int32_t pendingSwaps();
  private:
    // Letters we're currently interacting with
    // touch IDs mapped to controllers gathering points
    std::map<uint32_t, std::vector<LetterRef>>  mGroupedLetters;
    std::vector<LetterRef>                      mUngroupedLetters;
    LetterRef                                   mMovedLetter;
    void clearUngroupedLetters();
//    bool sharesNeighborWithUngroupedLetters( LetterRef letter ) const;
    bool isDirectNeighbor( LetterRef letter ) const;
    bool sharesNeighbor( LetterRef letter ) const;
    void updateView();
    std::vector<LetterRef>  ungroupedSharedNeighbors() const;
    std::vector<pk::Sprite> mSprites;
    pk::Sprite              mHorizontalSprite;
    pk::Sprite              mVerticalSprite;
    ci::Vec2i               mCellSize = ci::Vec2i::zero();
  };
}

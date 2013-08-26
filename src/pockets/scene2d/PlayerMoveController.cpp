//
//  PlayerMoveController.cpp
//  WordShift
//
//  Created by David Wicks on 5/13/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "PlayerMoveController.h"
#include "SwapStation.h"
#include "SoundStation.h"
#include "CollectionUtilities.hpp"
#include "GameColor.h"

using namespace cascade;
using namespace pockets;
using namespace cinder;
using namespace std;

PlayerMoveController::PlayerMoveController( const Sprite &horizontal, const Sprite &vertical ):
mHorizontalSprite( horizontal )
, mVerticalSprite( vertical )
{}

PlayerMoveController::~PlayerMoveController()
{}

void PlayerMoveController::updateWithLetter(uint32_t touch, LetterRef letter, bool canUseLetter )
{
  auto &letters = mGroupedLetters[touch];
  if( letters.empty() )
  { // kick things off if we have no letters
    if( canUseLetter )
    {
      if( !letter->isSelected() )
      { // if the letter isn't in use
        letter->setSelected( true );
        Sound().cueSound( SoundStation::eTileSelect, 0.0f );
        letters.push_back( letter );
      }
      else if ( vector_contains( mUngroupedLetters, letter ) )
      { // if we're the ones using the letter
//        cout << "Grouping Usable Letter" << endl;
        vector_remove( &mUngroupedLetters, letter );
        letters.push_back( letter );
        mMovedLetter = letter;
      }
    }
    else if ( vector_contains( mUngroupedLetters, letter ) )
    { // If we already control this letter
//      cout << "Grouping Unusable Letter" << endl;
      vector_remove( &mUngroupedLetters, letter );
      letters.push_back( letter );
      mMovedLetter = letter;
    }
  }
  else if( vector_contains( letters, letter ) )
  { // check for backtracking if we have more than one letter
    if( letters.size() > 1 && letter == letters.at( letters.size() - 2 ) )
    { // remove letter at end of list
      letters.back()->setSelected( false );
      Sound().cueSound( SoundStation::eTileDeselect, 0.0f );
      letters.pop_back();
    }
  }
  else if( !letter->isSelected() )
  { // check whether free letter is a neighbor
    if( canUseLetter && letters.back()->isNeighbor( letter ) )
    {
      letter->setSelected( true );
      Sound().cueSound( SoundStation::eTileSelect, 0.0f );
      letters.push_back( letter );
    }
  }
  else if( vector_contains( mUngroupedLetters, letter ) )
  { // check whether selected (but ungrouped) letter is a neighbor
    if( letters.back()->isNeighbor( letter ) )
    { // skips the canUseLetter check since it's already in use
//      cout << "Grouping Letter without marking as moved" << endl;
      vector_remove( &mUngroupedLetters, letter );
      letters.push_back( letter );
      Sound().cueSound( SoundStation::eTileSelect, 0.0f );
    }
  }
  if( letters.size() > 1 )
  {
    clearUngroupedLetters();
  }

  SwapStation::instance().setPendingUserSwaps( pendingSwaps() );
  updateView();
}

void PlayerMoveController::apply(uint32_t touch, LetterRef touch_letter, bool canUseLetter)
{
  auto &letters = mGroupedLetters[touch];

  if( letters.empty() && touch_letter )
  {
    if( isDirectNeighbor( touch_letter ) )
    {
      mUngroupedLetters.push_back( touch_letter );
      Sound().cueSound( SoundStation::eTileSelect, 0.0f );
//      cout << "Adding direct neighbor tile to selection so game can end" << endl;
    }
  }
  else if( letters.size() == 1 )
  { // Either deselect or move over to ungrouped letters
    if( letters.front() == mMovedLetter )
    { // already selected before, deselect
      letters.front()->setSelected( false );
      Sound().cueSound( SoundStation::eTileDeselect, 0.0f );
//      cout << "Deselecting from 1-element set" << endl;
    }
    else if( canUseLetter )
    {
      if( mUngroupedLetters.empty() || isDirectNeighbor( letters.front() ) || sharesNeighbor( letters.front() ) )
      { // if alone or any kind of neighbor
//        cout << "UNgrouping letter" << endl;
        mUngroupedLetters.push_back( letters.front() );
      }
      else
      { // not a neighbor
        letters.front()->setSelected( false );
        Sound().cueSound( SoundStation::eTileDeselect, 0.0f );
//        cout << "Deselecting item too far from others." << endl;
      }
    }
    else if( isDirectNeighbor( letters.front() ) )
    { // if a neighbor that will force a swap
//      cout << "UNgrouping direct neighbor" << endl;
      mUngroupedLetters.push_back( letters.front() );
    }
    else
    { // just deselect the letter (out on its own, or out of space)
      letters.front()->setSelected( false );
      Sound().cueSound( SoundStation::eTileDeselect, 0.0f );
//      cout << "Deselecting invalid solo item." << endl;
    }
  }
  else if( letters.size() > 1 )
  { // Swap all the letters in this group
    for( auto &letter : letters )
    {
      letter->setSelected( false );
    }
    vector<SwapStation::Swap> swaps;
    auto current = letters.begin();
    auto next = current + 1;
    float delay = 0.0f;
    while( next != letters.end() )
    {
      swaps.push_back( SwapStation::createSwap( *current, *next, delay ) );
      current++;
      next++;
      float inc = math<float>::clamp( lmap( delay, 0.0f, 0.32f, 0.084f, 0.05f ), 0.05f, 0.084f );
      delay += inc;
    }
    SwapStation::instance().performSwaps( swaps, true );
  }

  mGroupedLetters.erase( touch );
  mMovedLetter.reset();

  // Swap all ungrouped letters
  float delay = 0.0f;
  vector<SwapStation::Swap> ungrouped_swaps;
  for( int i = mUngroupedLetters.size() - 1; i > 0; --i )
  {
    for( int j = i - 1; j >= 0; --j )
    {
      if( mUngroupedLetters.at( i )->isNeighbor( mUngroupedLetters.at( j ) ) )
      {
        ungrouped_swaps.push_back( SwapStation::createSwap( mUngroupedLetters[i], mUngroupedLetters[j], delay ) );
        float inc = math<float>::clamp( lmap( delay, 0.0f, 0.32f, 0.084f, 0.05f ), 0.05f, 0.084f );
        delay += inc;
      }
    }
  }
  if( !ungrouped_swaps.empty() )
  {
    SwapStation::instance().performSwaps( ungrouped_swaps, true );
    clearUngroupedLetters();
  }
  SwapStation::instance().setPendingUserSwaps( pendingSwaps() );
  updateView();
}

void PlayerMoveController::updateView()
{
  mSprites.clear();
  for( auto &pair : mGroupedLetters )
  { // handle all potential sequenced swaps
    auto &letters = pair.second;
    auto current = letters.begin();
    auto next = current + 1;
    while( next < letters.end() )
    {
      Vec2i a = (*current)->getPosition();
      Vec2i b = (*next)->getPosition();
      Vec2i intersection = (a + b) * mCellSize / 2;
      Sprite sprite = (a.x != b.x) ? mHorizontalSprite : mVerticalSprite;
      sprite.setLoc( intersection );
      sprite.setTint( getBackgroundColor() );
      mSprites.push_back( sprite );
      current = next;
      ++next;
    }
  }
  // handle ungrouped swaps
  const auto &shared_neighbors = ungroupedSharedNeighbors();
  for( auto &letter : mUngroupedLetters )
  {
    const Vec2i a = letter->getPosition();
    const auto &neighbors = letter->getNeighbors();
    for( auto &neighbor : neighbors )
    {
      if( vector_contains( shared_neighbors, neighbor ) )
      {
          Vec2i b = neighbor->getPosition();
          Vec2i intersection = (a + b) * mCellSize / 2;
          Sprite sprite = (a.x != b.x) ? mHorizontalSprite : mVerticalSprite;
          sprite.setLoc( intersection );
          sprite.setTint( getBackgroundColor() );
          mSprites.push_back( sprite );
      }
    }
  }
}

void PlayerMoveController::render()
{
  for( auto &sprite : mSprites )
  {
    sprite.render();
  }
}

bool PlayerMoveController::sharesNeighbor(LetterRef letter) const
{
  for( LetterRef l : ungroupedSharedNeighbors() )
  { // does it share a neighbor with all other ungrouped letters?
    if( letter->isNeighbor( l ) )
    {
      return true;
    }
  }
  return false;
}

bool PlayerMoveController::isDirectNeighbor(LetterRef letter) const
{
  for( LetterRef l : mUngroupedLetters )
  { // is it a direct neighbor?
    if( l->isNeighbor( letter ) )
    {
      return true;
    }
  }
  return false;
}

vector<LetterRef> PlayerMoveController::ungroupedSharedNeighbors() const
{
  vector<LetterRef> neighbors;
  auto iter = mUngroupedLetters.begin();
  if( iter != mUngroupedLetters.end() )
  {
    neighbors = (*iter)->getNeighbors();
    auto null_ptr = [](LetterRef letter){ return !letter; };
    vector_erase_if( &neighbors, null_ptr );
    while( ++iter != mUngroupedLetters.end() )
    {
      vector<LetterRef> other_neighbors = (*iter)->getNeighbors();
      auto neg_compare = [=](LetterRef letter){ return !vector_contains(other_neighbors, letter); };
      vector_erase_if( &neighbors, neg_compare );
    }
  }
  return neighbors;
}

void PlayerMoveController::clearUngroupedLetters()
{
  for( auto &l : mUngroupedLetters )
  {
    l->setSelected( false );
  }
  mUngroupedLetters.clear();
}

int32_t PlayerMoveController::pendingSwaps()
{
  int32_t ret = 0;
  for( auto &pair : mGroupedLetters )
  { // at least one here
    ret += math<int32_t>::max(pair.second.size() - 1, 0);
  }
  ret += mUngroupedLetters.size();
  return ret;
}

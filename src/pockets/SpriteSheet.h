/*
 * Copyright (c) 2013, David Wicks
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <map>
#include "cinder/gl/Texture.h"
#include "cinder/Json.h"
#include "Pockets.h"

// TODO: add to CollectionUtils, move those to Pockets
template<typename K, typename V>
std::vector<K> map_keys( const std::map<K, V> &map )
{
  std::vector<K> ret;
  for( auto &pair : map )
  {
    ret.push_back( pair.first );
  }
  return ret;
}

/**
Loads a SpriteSheet as generated by SpriteSheetGenerator
Enables display of images from the sheet by id.
Duplicate ids will overwrite each other, resulting in only one sprite showing.

For custom JSON formats, pass in a ParseFunction to translate from the input JSON
description to the internal SpriteData.

Assumes bitmap and description are correctly related to each other.
Assumes you are providing valid keys/SpriteData to the sheet when drawing.

Not implemented: constructor/parse function pair for XML formats
*/

namespace pockets
{
typedef std::shared_ptr<class SpriteSheet> SpriteSheetRef;
class SpriteSheet
{
public:
  class SpriteData
  {
  public:
    SpriteData(){}; // need default constructor to use in a std::map
    //! construct sprite data from its pixel size and normalized texture coordinates
    SpriteData( const ci::Vec2i &pixel_size, const ci::Rectf &textureBounds, const ci::Vec2i &registration_point=ci::Vec2i::zero() );
    //! returns the Sprite's pixel dimensions
    inline ci::Vec2i getSize() const { return mSize; }
    //! returns the normalized texture coordinates of the sprite graphic
    inline ci::Rectf getTextureBounds() const { return mTextureBounds; }
    //! returns the registration point of the sprite, treated as the origin of the artwork
    inline ci::Vec2i getRegistrationPoint() const { return mRegistrationPoint; }
  private:
    ci::Vec2i mRegistrationPoint;
    ci::Vec2i mSize = ci::Vec2i::zero();
    ci::Rectf mTextureBounds = ci::Rectf(0,0,0,0);
  };
  typedef std::pair<std::string, SpriteData> NameDataPair;
  typedef std::vector<NameDataPair> SpriteDataCollection;
  //! ParseFunctions return a vector of name/SpriteData pairs from the JsonTree description
  typedef std::function<SpriteDataCollection (const ci::JsonTree &)>  ParseFunction;
  //! Generate a spritesheet from a source bitmap and description.
  //! Description is parsed according to the parser function.
	SpriteSheet( const ci::Surface &images, const ci::JsonTree &description, ParseFunction parser=&defaultParseFunction );
	~SpriteSheet();
  //! enable texturing and bind our texture
  void        enableAndBind();
  //! bind the spritesheet's texture
  inline void bind(){ mTexture.bind(); }
  //! unbind the spritesheet's texture
  inline void unbind(){ mTexture.unbind(); }

  //! draw named sprite with registration point at location
  void        draw( const std::string &sprite_name, const ci::Vec2f &loc );
  void        draw( const SpriteData &sprite, const ci::Vec2f &loc );
  //! draw sprite at location with texture scroll; for sliding texture within bounds (like a rectangular mask)
  void        drawScrolled( const std::string &sprite_name, const ci::Vec2f &loc, const ci::Vec2f &scroll );
  void        drawScrolled( const SpriteData &sprite, const ci::Vec2f &loc, const ci::Vec2f &scroll );
  //! draws sprite clipped to rect
  void        drawInRect( const std::string &sprite_name, const ci::Vec2f &loc, const ci::Rectf &bounding_rect );
  void        drawInRect( const SpriteData &sprite, const ci::Vec2f &loc, const ci::Rectf &bounding_rect );
  //! draw portion of sprite at location (used by drawInRect)
  void        drawPortion( const SpriteData &sprite, const ci::Vec2f &loc, const ci::Rectf &portion );
  //! returns a collection of all the sprites names; not in any order
  std::vector<std::string>  getSpriteNames(){ return map_keys( mSpriteData ); }
  //! get information about the named sprite. Useful for getting the size of the sprite for alignment purposes.
  inline const SpriteData&  getSpriteData( const std::string &sprite_name ){ return mSpriteData[sprite_name]; }
  static SpriteSheetRef  load( const ci::fs::path &base_path, const std::string &file_base_name );
private:
// map name to texture coordinates
  std::map<std::string, SpriteData>   mSpriteData;
  ci::gl::Texture                     mTexture;
  //! parser for format generated by accompanying SpriteSheetGenerator.app
  static SpriteDataCollection defaultParseFunction( const ci::JsonTree &input );
};
} // pockets::
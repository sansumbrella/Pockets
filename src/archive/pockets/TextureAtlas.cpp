/*
 * Copyright (c) 2013 David Wicks, sansumbrella.com
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

#include "pockets/TextureAtlas.h"
#include "pockets/CollectionUtilities.hpp"

#include "cinder/Json.h"
#include "cinder/gl/Texture.h"

using namespace std;
using namespace cinder;
using namespace pockets;

TextureAtlas::TextureAtlas( const Surface &images, const ci::JsonTree &description )
{
  gl::Texture::Format format;
  format.enableMipmapping();  // looks like my additions of num levels aren't in Cinder yet...

  mTexture = gl::Texture::create( images, format );
  parseDescription( description );
}

TextureAtlas::TextureAtlas( const Channel &images, const JsonTree &description )
{
  gl::Texture::Format format;
  format.enableMipmapping();
  format.setSwizzleMask( GL_RED, GL_RED, GL_RED, GL_RED );

  mTexture = gl::Texture::create( images, format );
  parseDescription( description );
}

void TextureAtlas::parseDescription( const ci::JsonTree &description )
{
  JsonTree sprites = description["sprites"];
  JsonTree meta = description["meta"];
  ivec2 bitmap_size( meta["width"].getValue<int>(), meta["height"].getValue<int>() );

  for( const auto &child : sprites )
  {
    Rectf bounds( child["x1"].getValue<int>(), child["y1"].getValue<int>(),
                  child["x2"].getValue<int>(), child["y2"].getValue<int>() );

    ivec2 registration_point( child["rx"].getValue<float>(), child["ry"].getValue<float>() );

    string id = child["id"].getValue();

    mData[ id ] = SpriteData{ { bounds.getUpperLeft() / vec2(bitmap_size), bounds.getLowerRight() / vec2(bitmap_size) },
      bounds.getSize(),
      registration_point };
  }
}

TextureAtlasUniqueRef TextureAtlas::create( const ci::Surface &images, const ci::JsonTree &description )
{
  return TextureAtlasUniqueRef( new TextureAtlas( images, description ) );
}

vector<string> TextureAtlas::getKeys()
{
  return map_keys( mData );
}

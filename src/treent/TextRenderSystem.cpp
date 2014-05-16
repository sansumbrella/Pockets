/*
 * Copyright (c) 2014 David Wicks, sansumbrella.com
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

#include "treent/TextRenderSystem.h"
#include "treent/LocationComponent.h"

#include "cinder/app/App.h"

using namespace std;
using namespace cinder;

namespace treent
{

TextComponent::TextComponent( gl::TextureFontRef font, const string &text ):
  _font( font )
{
  setText( text );
}

void TextComponent::setText( const std::string &text )
{
  gl::TextureFont::DrawOptions opt;
  opt.scale( 1.0f / app::getWindow()->getContentScale() ).pixelSnap( false );
  _glyph_placements = _font->getGlyphPlacements( text, opt );
}

//
//  MARK: - TextRenderSystem
//

void TextRenderSystem::update( EntityManagerRef entities, EventManagerRef events, double dt )
{
  _grouped_text.clear();
  for( auto entity : entities->entities_with_components<LocationComponent, TextComponent>() )
  {
    LocationComponentRef location;
    TextComponentRef     text;
    entity.unpack( location, text );

    _grouped_text[text->_font].push_back( make_pair( location, text ) );
  }
}

void TextRenderSystem::draw() const
{
  for( auto &pair : _grouped_text )
  {
    // TODO: custom extension of TextureFont so we can bind each font texture once per draw.
    auto &blocks = pair.second;
    for( auto &pair : blocks )
    {
      LocationComponentRef location = pair.first;
      TextComponentRef text = pair.second;

      gl::ScopedModelMatrix matrix;
      gl::multModelMatrix( Matrix44f( location->matrix ) );
      text->_font->drawGlyphs( text->_glyph_placements, Vec2f::zero() );
    }
  }
}

} // treent::



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

#pragma once

#include "treent/Treent.h"
#include "cinder/gl/TextureFont.h"

namespace treent
{

typedef std::vector<std::pair<uint16_t, ci::vec2>> GlyphPlacements;
typedef std::shared_ptr<struct TextComponent>       TextComponentRef;

struct TextComponent : Component<TextComponent>
{
  TextComponent() = default;
  TextComponent( ci::gl::TextureFontRef font, const std::string &text );

  //! Generates glyph placements for \a text. This is an optimization over storing the string.
  void setText( const std::string &text );

  ci::ColorA              color = ci::ColorA::white();
  ci::gl::TextureFontRef  _font;
  GlyphPlacements         _glyph_placements;
};

class TextRenderSystem : public System<TextRenderSystem>
{
public:
  void update( EntityManagerRef entities, EventManagerRef events, double dt ) override;
  void draw() const;
private:
  std::map<ci::gl::TextureFontRef, std::vector<std::pair<LocationComponentRef, TextComponentRef>>> _grouped_text;
};

} // treent::


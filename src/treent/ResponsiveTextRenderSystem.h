/*
 * Adapted version of TextRenderSystem that responds to and reformats according to 
 * text box size. Also allows customization of other text style options.
 */

#pragma once

#include "treent/Treent.h"
#include "cinder/gl/TextureFont.h"

namespace treent
{

typedef std::vector<std::pair<uint16_t, ci::Vec2f>> GlyphPlacements;
typedef std::shared_ptr<struct RespTextComponent>       RespTextComponentRef;

struct RespTextComponent : Component<RespTextComponent>
{
	RespTextComponent( ) = default;
	RespTextComponent( ci::gl::TextureFontRef font, const std::string &text, const float rank, const ci::ColorA col );

  //! Generates glyph placements for \a text. This is an optimization over storing the string.
  void setText( const std::string &text );

  ci::TextLayout		_layout;
  ci::ColorA			_color;
  ci::gl::TextureFontRef	_font;
  float					_ranking;
  GlyphPlacements		_glyph_placements;
};

class ResponsiveTextRenderSystem : public System<ResponsiveTextRenderSystem>
{
public:
  void update( EntityManagerRef entities, EventManagerRef events, double dt ) override;
  void draw() const;
private:
	std::map<ci::gl::TextureFontRef, std::vector<std::pair<LocationComponentRef, RespTextComponentRef>>> _grouped_text;
};

} // treent::


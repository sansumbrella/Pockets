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

  //! Reflows text within a TextLayout as value for that text changes
  void reflowLayout( float val, const std::string &text );

  //! Splits lines in a block of text based on a per-line character limit
  void splitLines( const std::string &text, int charLimit );

  //! Find longest word in the input string and use that to set max number of characters in a line
  int setMaxChars( const std::string &text );

  std::vector<ci::gl::TextureFont::DrawOptions>	_opts;	// vector of DrawOptions for each line of text
  std::vector<GlyphPlacements>		_glyph_placements; // vector of GlyphPlacements for each line
  float				_line_height;	// set height of each line of text
  ci::ColorA			_color;
  ci::gl::TextureRef	_texture;
  ci::gl::TextureFontRef	_font;
  float					_value;
  float					_rect_width; // width of current text box rectangle
  float					_rect_height; // height of current text box rectangle
  float					_base_rect_width; // width of base text box rectangle
  float					_base_rect_height; // height of base text box rectangle
  float					_base_font_size;	// base font size of text
  float					_line_aspect_ratio; // ideal line aspect ratio
  int					_max_chars;	// maximum number of characters in a line
};

class ResponsiveTextRenderSystem : public System<ResponsiveTextRenderSystem>
{
public:
  void update( EntityManagerRef entities, EventManagerRef events, double dt ) override;
  void draw() const;

  float					_aspect_ratio = 0.519; // Default value for now

private:
	std::map<ci::gl::TextureFontRef, std::vector<std::pair<LocationComponentRef, RespTextComponentRef>>> _grouped_text;

};

} // treent::


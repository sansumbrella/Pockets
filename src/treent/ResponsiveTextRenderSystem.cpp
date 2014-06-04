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

#include "treent/ResponsiveTextRenderSystem.h"
#include "treent/LocationComponent.h"
#include <boost/algorithm/string.hpp>
#include "cinder/app/App.h"
#include "cinder/Rand.h"

using namespace std;
using namespace cinder;

namespace treent
{

	RespTextComponent::RespTextComponent( ci::gl::TextureFontRef font, const string &text, const float &rank, const ci::ColorA &col, const ci::Vec2f &vel ) :
  _font( font ),
  _color( col ),
  _curr_value( rank ),
  _prev_value( rank ),
  _velocity( vel )
{
	// dimensions of text box, increased based on rank
	_rect_width = 50.0f;
	_rect_height = 100.0f;

	// default dimensions of text box and font
	_base_rect_width = _rect_width;
	_base_rect_height = _rect_height;
	_base_font_size = _font->getFont().getSize();

	// ideal max number of characters is a default value for now
	_max_chars = 12; // setMaxChars( text );
  _line_aspect_ratio = _max_chars * 0.519; // using a default font aspect ratio value for now
  reflowLayout( rank, text );
}

void RespTextComponent::setText( const std::string &text )
{
	gl::TextureFont::DrawOptions opt;
	opt.scale( 1.0f / app::getWindow()->getContentScale() ).pixelSnap( false );
	auto gp = _font->getGlyphPlacements( text, opt );
}

int RespTextComponent::setMaxChars( const std::string &text )
{
	string workingWord = "";  //working string
	string maxWord = "";  //biggest word 

	// find longest word in string
	for (int i = 0; i < text.size( ); i++)
	{
		//If it's not a space, semicolon, colon, or comma, add it to the word, if it is, reset workingWord
		if (text[i] != ' ' && text[i] != ';' && text[i] != ':' && text[i] != ',')   
		{	workingWord += text[i]; }
		else 
		{	workingWord = ""; }

		// If workingword > maxWord, we have a new maxWord
		if (workingWord.size( ) > maxWord.size( ))
		{	maxWord = workingWord; }
	}

	return maxWord.size();
}

// split the headline into lines based on character count and word length
void RespTextComponent::splitLines( const std::string &text, int charLimit )
{
	// split text into vector of words
	vector<std::string> words;
	boost::split( words, text, boost::is_any_of( " " ) );

	std::string line = ""; // line to be added, filled with default text 

	std::string currWord = ""; // current word
	std::string remainder = ""; // remainder of original text
	std::string nextTwoWords = ""; // next two words
	int wordIndex = 0;
	
	if ( wordIndex < words.size( ) )
	{
		// currWord is always one word behind nextTwoWords
		// add words to both until the length of one string is less than charLimit
        // per line, while the length of the other is greater than charLimit
		while (nextTwoWords.length() < charLimit)
		{
			currWord = nextTwoWords;
			nextTwoWords += words[wordIndex] + " ";
			wordIndex++;
			if (wordIndex >= words.size())
				break;
		}
		
		// add a line for whichever string is closer to the character limit
		if ((math<int>::abs( currWord.length() - charLimit ) < math<int>::abs( nextTwoWords.length() - charLimit )) && nextTwoWords.length() > 2)
		{
			line = currWord;
			wordIndex--;
		}
		else
		{
			line = nextTwoWords;
		}

		gl::TextureFont::DrawOptions opt;

		float lineSize = line.size( );
		float lineScale = lmap( lineSize, 1.0f, (float) charLimit, 0.1f, 1.0f );
		opt.scale( lineScale ).pixelSnap( true );
		auto gp = _font->getGlyphPlacements( line, Rectf( 0, 0, _rect_width, _line_height*2 ),opt );
		_glyph_placements.push_back( gp );
		_opts.push_back( opt );

		// repeat with the remainder of the original string
		for (int i = wordIndex; i < words.size(); i++)
		{
			remainder += words[i] + " ";
		}

		if (remainder.size() > charLimit)
		{
			splitLines( remainder, charLimit );
		}

		// if the length of the text is less than charLimit, add 
		// this as the last line.
		else
		{
			float lineSize = remainder.size();
			float lineScale = lmap( lineSize, 1.0f, (float) charLimit, 0.1f, 1.0f );

			opt.scale( lineScale ).pixelSnap( true );
			app::console() << "scale: " << opt.getScale() << endl;
			auto gp = _font->getGlyphPlacements( remainder, Rectf( 0, 0, _rect_width, _line_height * 2 ),opt );
			_glyph_placements.push_back( gp );
			_opts.push_back( opt );
		}
	}
}

void RespTextComponent::reflowLayout( float val, const std::string &text )
{
	// change rect dimensions & font size based on rank
	float scaler = lmap( val, 1.0f, 10.0f, 0.5f, 2.0f );
	
	// with the assumption that val > 0, scale text box width and height by scaler
	_rect_height = _base_rect_height * scaler;
	_rect_width = _base_rect_width * scaler;

	// scale font size with text box
	auto currfont = _font->getFont();
	_font = gl::TextureFont::create( Font( currfont.getName(), _base_font_size * scaler ) );


	auto ideal_line_height = _rect_width /( _line_aspect_ratio*scaler);
	int line_count = ceil( _rect_height / ideal_line_height );

	_line_height = _rect_height / line_count;

	app::console( ) << "line count: " << line_count << endl;
	app::console( ) << "text length: " << text.length() << endl;

	// target character count per line is length of total string divided by number of lines
	int char_ct_per_line = ceil( text.length() / line_count );

	if (char_ct_per_line < setMaxChars( text ))
	{
		// make sure the longest word in the headline is not longer than the 
		// per-line character count
		char_ct_per_line = setMaxChars( text );
	}
	app::console( ) << "char count: " << char_ct_per_line << endl;

	_opts.clear();
	_glyph_placements.clear();
	splitLines( text, char_ct_per_line);
}

//
//  MARK: - ResponsiveTextRenderSystem
//
void ResponsiveTextRenderSystem::update( EntityManagerRef entities, EventManagerRef events, double dt )
{
  _grouped_text.clear();
  for (auto entity : entities->entities_with_components<LocationComponent, RespTextComponent>( ))
  {
    LocationComponentRef location;
	RespTextComponentRef     text;
    entity.unpack( location, text );

    _grouped_text[text->_font].push_back( make_pair( location, text ) );
  }
}

void ResponsiveTextRenderSystem::draw( ) const
{
  for( auto &pair : _grouped_text )
  {
    // TODO: custom extension of TextureFont so we can bind each font texture once per draw.
    auto &blocks = pair.second;
    for( auto &pair : blocks )
    {
      LocationComponentRef location = pair.first;
	  RespTextComponentRef text = pair.second;

      gl::ScopedModelMatrix matrix;
      gl::multModelMatrix( Matrix44f( location->matrix ) );
	  
	  for (int i = 0; i < text->_glyph_placements.size(); i++)
	  {
		  auto gp = text->_glyph_placements[i];
		  gl::color( text->_color );
		
		  // need a good way to calculate line separation value
		  text->_font->drawGlyphs( gp, Vec2f( 0, (text->_font->getFont().getSize()) * i ), text->_opts[i] );
	  }
    }
  }
}

} // treent::



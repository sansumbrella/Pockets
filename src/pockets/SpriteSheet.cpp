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

#include "SpriteSheet.h"

using namespace std;
using namespace ci;

inline void drawRect( const ci::Rectf &position_rect, const ci::Rectf &tex_coord_rect )
{
  glEnableClientState( GL_VERTEX_ARRAY );
	GLfloat positions[8];
	glVertexPointer( 2, GL_FLOAT, 0, positions );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	GLfloat texCoords[8];
	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
	positions[0*2+0] = position_rect.getX2(); texCoords[0*2+0] = tex_coord_rect.getX2();
	positions[0*2+1] = position_rect.getY1(); texCoords[0*2+1] = tex_coord_rect.getY1();

	positions[1*2+0] = position_rect.getX1(); texCoords[1*2+0] = tex_coord_rect.getX1();
	positions[1*2+1] = position_rect.getY1(); texCoords[1*2+1] = tex_coord_rect.getY1();

  positions[2*2+0] = position_rect.getX2(); texCoords[2*2+0] = tex_coord_rect.getX2();
	positions[2*2+1] = position_rect.getY2(); texCoords[2*2+1] = tex_coord_rect.getY2();

  positions[3*2+0] = position_rect.getX1(); texCoords[3*2+0] = tex_coord_rect.getX1();
	positions[3*2+1] = position_rect.getY2(); texCoords[3*2+1] = tex_coord_rect.getY2();

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

SpriteSheet::SpriteData::SpriteData( const Vec2i &size, const Rectf &texture_bounds, const Vec2i &registration_point ):
mSize( size )
, mTextureBounds( texture_bounds )
, mRegistrationPoint( registration_point )
{}

SpriteSheet::SpriteSheet( const Surface &images, const JsonTree &description, ParseFunction parse )
{
  gl::Texture::Format format;
  format.setMagFilter( GL_NEAREST );
  format.setMinFilter( GL_NEAREST );
  mTexture = gl::Texture( images, format );
  SpriteDataCollection sprite_info = parse( description );
  for( auto &pair : sprite_info )
  {
    mSpriteData[pair.first] = pair.second;
  }
}

SpriteSheet::~SpriteSheet()
{}

SpriteSheet::SpriteDataCollection SpriteSheet::defaultParseFunction( const JsonTree &desc )
{
  SpriteDataCollection ret;
  JsonTree sprites = desc["sprites"];
  JsonTree meta = desc["meta"];
  Vec2i bitmap_size( meta["width"].getValue<int>(), meta["height"].getValue<int>() );
  for( const auto &child : sprites )
  {
    Rectf bounds( child["x1"].getValue<int>(), child["y1"].getValue<int>()
                 , child["x2"].getValue<int>(), child["y2"].getValue<int>() );
    string id = child["id"].getValue();
    SpriteData sprite( bounds.getSize()
                     , Rectf( bounds.getUpperLeft() / bitmap_size
                             , bounds.getLowerRight() / bitmap_size ) );
    ret.push_back( NameDataPair( id, sprite ) );
  }
  return ret;
}

void SpriteSheet::enableAndBind()
{
  gl::enable( GL_TEXTURE_2D );
  bind();
}

void SpriteSheet::draw(const string &sprite_name, const Vec2f &loc)
{
  draw( mSpriteData[sprite_name], loc );
}

void SpriteSheet::draw(const string &sprite_name, const Vec2f &loc, const Vec2f &offsets)
{
  draw( mSpriteData[sprite_name], loc, offsets );
}

void SpriteSheet::draw( const SpriteData &sprite, const Vec2f &loc )
{
  Rectf coords = sprite.getTextureBounds();
  Rectf rect( loc, loc + sprite.getSize() );

  drawRect( rect - sprite.getRegistrationPoint(), coords );
}

void SpriteSheet::draw( const SpriteData &sprite, const Vec2f &loc, const Vec2f &offsets )
{
  Rectf coords = sprite.getTextureBounds();
  Rectf rect( loc, loc + sprite.getSize() );
  // adjust for offsets
  if( offsets.x < 0 )
  {
    coords.x1 = lerp( coords.getX1(), coords.getX2(), -offsets.x );
    rect.x2 = loc.x + sprite.getSize().x * (1 + offsets.x);
  }
  else if( offsets.x > 0 )
  {
    coords.x2 = lerp( coords.getX1(), coords.getX2(), 1 - offsets.x );
    rect.x1 = loc.x + sprite.getSize().x * offsets.x;
  }
  if( offsets.y < 0 )
  {
    coords.y1 = lerp( coords.getY1(), coords.getY2(), -offsets.y );
    rect.y2 = loc.y + sprite.getSize().y * (1 + offsets.y);
  }
  else if( offsets.y > 0 )
  {
    coords.y2 = lerp( coords.getY1(), coords.getY2(), 1 - offsets.y );
    rect.y1 = loc.y + sprite.getSize().y * offsets.y;
  }

  drawRect( rect - sprite.getRegistrationPoint(), coords );
}

void SpriteSheet::drawInRect(const std::string &sprite_name, const ci::Vec2f &loc, const ci::Rectf &bounding_rect)
{
  drawInRect( getSpriteData(sprite_name), loc, bounding_rect );
}

void SpriteSheet::drawInRect( const SpriteData &sprite, const Vec2f &loc, const Rectf &bounding_rect )
{
  Rectf sprite_bounds( Vec2f::zero(), sprite.getSize() );
  sprite_bounds.offset( loc - sprite.getRegistrationPoint() );
  Rectf clipped_size = sprite_bounds.getClipBy( bounding_rect );
  clipped_size.offset( sprite.getRegistrationPoint() - loc );
  Rectf portion( clipped_size.getX1() / sprite.getSize().x, clipped_size.getY1() / sprite.getSize().y
                , clipped_size.getX2() / sprite.getSize().x, clipped_size.getY2() / sprite.getSize().y );
  draw( sprite, loc, portion );
}

void SpriteSheet::draw( const SpriteData &sprite, const Vec2f &loc, const Rectf &portion )
{
  Rectf tex_coords;
  tex_coords.x1 = lerp( sprite.getTextureBounds().getX1(), sprite.getTextureBounds().getX2(), portion.getX1() );
  tex_coords.x2 = lerp( sprite.getTextureBounds().getX1(), sprite.getTextureBounds().getX2(), portion.getX2() );
  tex_coords.y1 = lerp( sprite.getTextureBounds().getY1(), sprite.getTextureBounds().getY2(), portion.getY1() );
  tex_coords.y2 = lerp( sprite.getTextureBounds().getY1(), sprite.getTextureBounds().getY2(), portion.getY2() );

  Vec2f br( loc + sprite.getSize() );
  Rectf positions;
  positions.x1 = lerp( loc.x, br.x, portion.getX1() );
  positions.x2 = lerp( loc.x, br.x, portion.getX2() );
  positions.y1 = lerp( loc.y, br.y, portion.getY1() );
  positions.y2 = lerp( loc.y, br.y, portion.getY2() );

  drawRect( positions - sprite.getRegistrationPoint(), tex_coords );
}


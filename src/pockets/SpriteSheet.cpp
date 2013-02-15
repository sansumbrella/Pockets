//
//  SpriteSheet.cpp
//  SpriteSheetTester
//
//  Created by David Wicks on 2/14/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "SpriteSheet.h"

using namespace std;
using namespace ci;

SpriteSheet::SpriteData::SpriteData( const Vec2i &size, const Rectf &texture_bounds ):
mSize( size )
, mTextureBounds( texture_bounds )
{}

SpriteSheet::SpriteSheet( const Surface &images, const JsonTree &description, ParseFunction parse )
{
  mTexture = gl::Texture( images );
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

  glEnableClientState( GL_VERTEX_ARRAY );
	GLfloat verts[8];
	glVertexPointer( 2, GL_FLOAT, 0, verts );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	GLfloat texCoords[8];
	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
	verts[0*2+0] = rect.getX2(); texCoords[0*2+0] = coords.getX2();
	verts[0*2+1] = rect.getY1(); texCoords[0*2+1] = coords.getY1();

	verts[1*2+0] = rect.getX1(); texCoords[1*2+0] = coords.getX1();
	verts[1*2+1] = rect.getY1(); texCoords[1*2+1] = coords.getY1();

  verts[2*2+0] = rect.getX2(); texCoords[2*2+0] = coords.getX2();
	verts[2*2+1] = rect.getY2(); texCoords[2*2+1] = coords.getY2();

  verts[3*2+0] = rect.getX1(); texCoords[3*2+0] = coords.getX1();
	verts[3*2+1] = rect.getY2(); texCoords[3*2+1] = coords.getY2();

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
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

  glEnableClientState( GL_VERTEX_ARRAY );
	GLfloat verts[8];
	glVertexPointer( 2, GL_FLOAT, 0, verts );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	GLfloat texCoords[8];
	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
	verts[0*2+0] = rect.getX2(); texCoords[0*2+0] = coords.getX2();
	verts[0*2+1] = rect.getY1(); texCoords[0*2+1] = coords.getY1();

	verts[1*2+0] = rect.getX1(); texCoords[1*2+0] = coords.getX1();
	verts[1*2+1] = rect.getY1(); texCoords[1*2+1] = coords.getY1();

  verts[2*2+0] = rect.getX2(); texCoords[2*2+0] = coords.getX2();
	verts[2*2+1] = rect.getY2(); texCoords[2*2+1] = coords.getY2();

  verts[3*2+0] = rect.getX1(); texCoords[3*2+0] = coords.getX1();
	verts[3*2+1] = rect.getY2(); texCoords[3*2+1] = coords.getY2();

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}


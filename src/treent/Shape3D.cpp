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

#include "treent/Shape3D.h"
#include "pockets/TextureAtlas.h"

using namespace pockets;
using namespace cinder;

namespace treent
{

void Shape3D::transform(const ci::Matrix33f &mat)
{
  for( Vertex3D &v : vertices )
  {
    v.position = mat.transformVec( v.position );
  }
}

void Shape3D::setAsBox( const Rectf &bounds )
{
  if( vertices.size() != 4 ){ vertices.assign( 4, Vertex3D{} ); }
  vertices[0].position = Vec3f( bounds.getUpperRight(), 0.0f );
  vertices[1].position = Vec3f( bounds.getUpperLeft() );
  vertices[2].position = Vec3f( bounds.getLowerRight() );
  vertices[3].position = Vec3f( bounds.getLowerLeft() );
}

void Shape3D::setBoxTextureCoords( const SpriteData &sprite_data )
{
  vertices[0].tex_coord = sprite_data.texture_bounds.getUpperRight();
  vertices[1].tex_coord = sprite_data.texture_bounds.getUpperLeft();
  vertices[2].tex_coord = sprite_data.texture_bounds.getLowerRight();
  vertices[3].tex_coord = sprite_data.texture_bounds.getLowerLeft();
}

void Shape3D::matchTexture(const SpriteData &sprite_data)
{
  if( vertices.size() != 4 ){ vertices.assign( 4, Vertex3D{} ); }
  Rectf screen_bounds{ { 0.0f, 0.0f }, sprite_data.size };
  screen_bounds -= sprite_data.registration_point;
  vertices[0].position = Vec3f( screen_bounds.getUpperRight() );
  vertices[1].position = Vec3f( screen_bounds.getUpperLeft() );
  vertices[2].position = Vec3f( screen_bounds.getLowerRight() );
  vertices[3].position = Vec3f( screen_bounds.getLowerLeft() );
  vertices[0].tex_coord = sprite_data.texture_bounds.getUpperRight();
  vertices[1].tex_coord = sprite_data.texture_bounds.getUpperLeft();
  vertices[2].tex_coord = sprite_data.texture_bounds.getLowerRight();
  vertices[3].tex_coord = sprite_data.texture_bounds.getLowerLeft();
}

void Shape3D::setAsTriangle(const ci::Vec3f &a, const ci::Vec3f &b, const ci::Vec3f &c)
{
  if( vertices.size() != 3 ){ vertices.assign( 3, Vertex3D{} ); }
  vertices[0].position = a;
  vertices[1].position = b;
  vertices[2].position = c;
}

void Shape3D::setAsLine( const Vec2f &begin, const Vec2f &end, float width )
{
  Vec2f ray = end - begin;
  Vec2f side = ray.normalized() * width;
  Vec2f N( -side.y, side.x );
  Vec2f S = -N;

  if( vertices.size() != 4 )
  { vertices.assign( 4, Vertex3D{} ); }
  vertices.at(0).position = Vec3f( begin + S, 0.0f );
  vertices.at(1).position = Vec3f( begin + N, 0.0f );
  vertices.at(2).position = Vec3f( end + S, 0.0f );
  vertices.at(3).position = Vec3f( end + N, 0.0f );
}

void Shape3D::setAsCappedLine( const ci::Vec2f &begin, const ci::Vec2f &end, float width )
{
  Vec2f ray = end - begin;
  Vec2f cap = ray.normalized() * width;
  Vec2f N( -cap.y, cap.x );
  Vec2f S = -N;
  Vec2f NW = N - cap;
  Vec2f NE = N + cap;
  Vec2f SE = -NW;
  Vec2f SW = -NE;

  if( vertices.size() != 8 )
  { vertices.assign( 8, Vertex3D{} ); }
  vertices.at(0).position = Vec3f( begin + SW, 0.0f );
  vertices.at(1).position = Vec3f( begin + NW, 0.0f );
  vertices.at(2).position = Vec3f( begin + S, 0.0f );
  vertices.at(3).position = Vec3f( begin + N, 0.0f );
  vertices.at(4).position = Vec3f( end + S, 0.0f );
  vertices.at(5).position = Vec3f( end + N, 0.0f );
  vertices.at(6).position = Vec3f( end + SE, 0.0f );
  vertices.at(7).position = Vec3f( end + NE, 0.0f );
}

void Shape3D::setColor( const ColorA8u &color )
{
  for( auto &vert : vertices )
  {
    vert.color = color;
  }
}

} // treent::

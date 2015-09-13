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
  vertices[0].position = vec3( bounds.getUpperRight(), 0.0f );
  vertices[1].position = vec3( bounds.getUpperLeft() );
  vertices[2].position = vec3( bounds.getLowerRight() );
  vertices[3].position = vec3( bounds.getLowerLeft() );
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
  vertices[0].position = vec3( screen_bounds.getUpperRight() );
  vertices[1].position = vec3( screen_bounds.getUpperLeft() );
  vertices[2].position = vec3( screen_bounds.getLowerRight() );
  vertices[3].position = vec3( screen_bounds.getLowerLeft() );
  vertices[0].tex_coord = sprite_data.texture_bounds.getUpperRight();
  vertices[1].tex_coord = sprite_data.texture_bounds.getUpperLeft();
  vertices[2].tex_coord = sprite_data.texture_bounds.getLowerRight();
  vertices[3].tex_coord = sprite_data.texture_bounds.getLowerLeft();
}

void Shape3D::setAsTriangle(const ci::vec3 &a, const ci::vec3 &b, const ci::vec3 &c)
{
  if( vertices.size() != 3 ){ vertices.assign( 3, Vertex3D{} ); }
  vertices[0].position = a;
  vertices[1].position = b;
  vertices[2].position = c;
}

void Shape3D::setAsLine( const vec2 &begin, const vec2 &end, float width )
{
  vec2 ray = end - begin;
  vec2 side = ray.normalized() * width;
  vec2 N( -side.y, side.x );
  vec2 S = -N;

  if( vertices.size() != 4 )
  { vertices.assign( 4, Vertex3D{} ); }
  vertices.at(0).position = vec3( begin + S, 0.0f );
  vertices.at(1).position = vec3( begin + N, 0.0f );
  vertices.at(2).position = vec3( end + S, 0.0f );
  vertices.at(3).position = vec3( end + N, 0.0f );
}

void Shape3D::setAsCappedLine( const ci::vec2 &begin, const ci::vec2 &end, float width )
{
  vec2 ray = end - begin;
  vec2 cap = ray.normalized() * width;
  vec2 N( -cap.y, cap.x );
  vec2 S = -N;
  vec2 NW = N - cap;
  vec2 NE = N + cap;
  vec2 SE = -NW;
  vec2 SW = -NE;

  if( vertices.size() != 8 )
  { vertices.assign( 8, Vertex3D{} ); }
  vertices.at(0).position = vec3( begin + SW, 0.0f );
  vertices.at(1).position = vec3( begin + NW, 0.0f );
  vertices.at(2).position = vec3( begin + S, 0.0f );
  vertices.at(3).position = vec3( begin + N, 0.0f );
  vertices.at(4).position = vec3( end + S, 0.0f );
  vertices.at(5).position = vec3( end + N, 0.0f );
  vertices.at(6).position = vec3( end + SE, 0.0f );
  vertices.at(7).position = vec3( end + NE, 0.0f );
}

void Shape3D::setColor( const ColorA8u &color )
{
  for( auto &vert : vertices )
  {
    vert.color = color;
  }
}

} // treent::

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

#include "treent/ShapeComponent.h"
#include "pockets/TextureAtlas.h"

using namespace pockets;
using namespace cinder;

namespace treent
{

void ShapeComponent::transform(const ci::mat4 &mat)
{
  for( Vertex2D &v : vertices )
  {
    v.position = vec2( mat * vec4( v.position, 0.0f, 1.0f ) );
  }
}

void ShapeComponent::setAsCircle(const ci::vec2 &radius, float start_radians, float end_radians, size_t segments )
{
  if( segments < 2 ) { // based off of cinder, though we're less generous with the vertices
    segments = math<float>::floor( math<float>::max( radius.x, radius.y ) * abs(end_radians - start_radians) / 3 );
  }
  if( segments < 3 ){
    segments = 3;
  }
  if( vertices.size() != segments * 5 )
  {
    vertices.assign( segments * 5, Vertex2D{} );
  }
  vec2 a{ 0.0f, 0.0f };
  for( int i = 0; i < segments; ++i )
  {
    float t1 = lmap<float>( i, 0, segments, start_radians, end_radians );
    float t2 = lmap<float>( i + 1, 0, segments, start_radians, end_radians );
    vec2 b = vec2{ math<float>::cos( t1 ), math<float>::sin( t1 ) } * radius;
    vec2 c = vec2{ math<float>::cos( t2 ), math<float>::sin( t2 ) } * radius;
    vertices.at(i * 5 + 0).position = a;
    vertices.at(i * 5 + 1).position = b;
    vertices.at(i * 5 + 2).position = c;
    vertices.at(i * 5 + 3).position = c;
    vertices.at(i * 5 + 4).position = a;
  }
}

void ShapeComponent::setAsBox( const Rectf &bounds )
{
  if( vertices.size() != 4 ){ vertices.assign( 4, Vertex2D{} ); }
  vertices[0].position = bounds.getUpperRight();
  vertices[1].position = bounds.getUpperLeft();
  vertices[2].position = bounds.getLowerRight();
  vertices[3].position = bounds.getLowerLeft();
}

void ShapeComponent::setBoxTextureCoords( const SpriteData &sprite_data )
{
  vertices[0].tex_coord = sprite_data.texture_bounds.getUpperRight();
  vertices[1].tex_coord = sprite_data.texture_bounds.getUpperLeft();
  vertices[2].tex_coord = sprite_data.texture_bounds.getLowerRight();
  vertices[3].tex_coord = sprite_data.texture_bounds.getLowerLeft();
}

void ShapeComponent::matchTexture(const SpriteData &sprite_data)
{
  if( vertices.size() != 4 ){ vertices.assign( 4, Vertex2D{} ); }
  Rectf screen_bounds{ { 0.0f, 0.0f }, sprite_data.size };
  screen_bounds -= sprite_data.registration_point;
  vertices[0].position = screen_bounds.getUpperRight();
  vertices[1].position = screen_bounds.getUpperLeft();
  vertices[2].position = screen_bounds.getLowerRight();
  vertices[3].position = screen_bounds.getLowerLeft();
  vertices[0].tex_coord = sprite_data.texture_bounds.getUpperRight();
  vertices[1].tex_coord = sprite_data.texture_bounds.getUpperLeft();
  vertices[2].tex_coord = sprite_data.texture_bounds.getLowerRight();
  vertices[3].tex_coord = sprite_data.texture_bounds.getLowerLeft();
}

void ShapeComponent::setAsTriangle(const ci::vec2 &a, const ci::vec2 &b, const ci::vec2 &c)
{
  if( vertices.size() != 3 ){ vertices.assign( 3, Vertex2D{} ); }
  vertices[0].position = a;
  vertices[1].position = b;
  vertices[2].position = c;
}

void ShapeComponent::setAsLine( const vec2 &begin, const vec2 &end, float width )
{
  vec2 ray = end - begin;
  vec2 side = normalize( ray ) * width;
  vec2 N( -side.y, side.x );
  vec2 S = -N;

  if( vertices.size() != 4 )
  { vertices.assign( 4, Vertex2D{} ); }
  vertices.at(0).position = begin + S;
  vertices.at(1).position = begin + N;
  vertices.at(2).position = end + S;
  vertices.at(3).position = end + N;
}

void ShapeComponent::setAsCappedLine( const ci::vec2 &begin, const ci::vec2 &end, float width )
{
  vec2 ray = end - begin;
  vec2 cap = normalize( ray ) * width;
  vec2 N( -cap.y, cap.x );
  vec2 S = -N;
  vec2 NW = N - cap;
  vec2 NE = N + cap;
  vec2 SE = -NW;
  vec2 SW = -NE;

  if( vertices.size() != 8 )
  { vertices.assign( 8, Vertex2D{} ); }
  vertices.at(0).position = begin + SW;
  vertices.at(1).position = begin + NW;
  vertices.at(2).position = begin + S;
  vertices.at(3).position = begin + N;
  vertices.at(4).position = end + S;
  vertices.at(5).position = end + N;
  vertices.at(6).position = end + SE;
  vertices.at(7).position = end + NE;
}

void ShapeComponent::setColor( const ColorA8u &color )
{
  for( auto &vert : vertices )
  {
    vert.color = color;
  }
}

} // treent::

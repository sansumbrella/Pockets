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

#include "pockets/rendering/RenderMesh.h"

using namespace pockets;
using namespace cinder;

void RenderMesh::transform(const ci::MatrixAffine2f &mat)
{
  for( Vertex &v : vertices )
  {
    v.position = mat.transformVec( v.position );
  }
}

void RenderMesh::setAsCircle(const ci::Vec2f &radius, float start_radians, float end_radians, size_t segments )
{
  if( segments < 2 ) { // based off of cinder, though we're less generous with the vertices
    segments = math<float>::floor( math<float>::max( radius.x, radius.y ) * abs(end_radians - start_radians) / 3 );
  }
  if( segments < 3 ){
    segments = 3;
  }
  if( vertices.size() != segments * 5 )
  {
    vertices.assign( segments * 5, Vertex{} );
  }
  Vec2f a{ 0.0f, 0.0f };
  for( int i = 0; i < segments; ++i )
  {
    float t1 = lmap<float>( i, 0, segments, start_radians, end_radians );
    float t2 = lmap<float>( i + 1, 0, segments, start_radians, end_radians );
    Vec2f b = Vec2f{ math<float>::cos( t1 ), math<float>::sin( t1 ) } * radius;
    Vec2f c = Vec2f{ math<float>::cos( t2 ), math<float>::sin( t2 ) } * radius;
    vertices.at(i * 5 + 0).position = a;
    vertices.at(i * 5 + 1).position = b;
    vertices.at(i * 5 + 2).position = c;
    vertices.at(i * 5 + 3).position = c;
    vertices.at(i * 5 + 4).position = a;
  }
}

void RenderMesh::setAsBox( const Rectf &bounds )
{
  if( vertices.size() != 4 ){ vertices.assign( 4, Vertex{} ); }
  vertices[0].position = bounds.getUpperRight();
  vertices[1].position = bounds.getUpperLeft();
  vertices[2].position = bounds.getLowerRight();
  vertices[3].position = bounds.getLowerLeft();
}

/*
void RenderMesh::setBoxTextureCoords( const SpriteData &sprite_data )
{
  vertices[0].tex_coord = sprite_data.texture_bounds.getUpperRight();
  vertices[1].tex_coord = sprite_data.texture_bounds.getUpperLeft();
  vertices[2].tex_coord = sprite_data.texture_bounds.getLowerRight();
  vertices[3].tex_coord = sprite_data.texture_bounds.getLowerLeft();
}

void RenderMesh::matchTexture(const SpriteData &sprite_data)
{
  if( vertices.size() != 4 ){ vertices.assign( 4, Vertex{} ); }
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
*/

void RenderMesh::setAsTriangle(const ci::Vec2f &a, const ci::Vec2f &b, const ci::Vec2f &c)
{
  if( vertices.size() != 3 ){ vertices.assign( 3, Vertex{} ); }
  vertices[0].position = a;
  vertices[1].position = b;
  vertices[2].position = c;
}

void RenderMesh::setAsLine( const Vec2f &begin, const Vec2f &end, float width )
{
  Vec2f ray = end - begin;
  Vec2f side = ray.normalized() * width;
  Vec2f N( -side.y, side.x );
  Vec2f S = -N;

  if( vertices.size() != 4 )
  { vertices.assign( 4, Vertex{} ); }
  vertices.at(0).position = begin + S;
  vertices.at(1).position = begin + N;
  vertices.at(2).position = end + S;
  vertices.at(3).position = end + N;
}

void RenderMesh::setAsCappedLine( const ci::Vec2f &begin, const ci::Vec2f &end, float width )
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
  { vertices.assign( 8, Vertex{} ); }
  vertices.at(0).position = begin + SW;
  vertices.at(1).position = begin + NW;
  vertices.at(2).position = begin + S;
  vertices.at(3).position = begin + N;
  vertices.at(4).position = end + S;
  vertices.at(5).position = end + N;
  vertices.at(6).position = end + SE;
  vertices.at(7).position = end + NE;
}

void RenderMesh::setColor( const ColorA8u &color )
{
  for( auto &vert : vertices )
  {
    vert.color = color;
  }
}

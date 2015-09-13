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

#pragma once
#include "treent/Treent.h"
#include "cinder/Color.h"
#include "cinder/Rect.h"
#include "cinder/Matrix.h"

namespace pockets
{
  struct SpriteData;
}

namespace treent
{

struct Vertex3D
{
  Vertex3D() = default;
  Vertex3D( const ci::vec3 &position, const ci::ColorA8u &color, const ci::vec2 &uv ):
    position( position ),
    color( color ),
    tex_coord( uv )
  {}
  ci::vec3     position  = ci::vec3( 0 );
  ci::ColorA8u  color     = ci::ColorA8u::white();
  ci::vec2     tex_coord = ci::vec2( 0 );
};

/**
 Shape3D:
 Collection of vertices suitable for rendering as a triangle strip.
 Drawn by the RenderSystem
 Updated by SpriteAnimationSystem or user Scripts
 Member functions let you conveniently set it to various shapes:
 - Box
 - Circle
 - Ribbon
 - Line
 - Texture billboard (special case of Box)
 Additional methods ease the texturing of those shapes.
 */
typedef std::shared_ptr<struct Shape3D> Shape3DRef;
struct Shape3D : Component<Shape3D>
{
  Shape3D( int vertex2D_count=3 )
  {
    vertices.assign( vertex2D_count, Vertex3D{} );
  }

  //! vertices in triangle_strip order
  std::vector<Vertex3D> vertices;

  //! Set the mesh bounds to a box shape
  void setAsBox( const ci::Rectf &bounds );

  //! Set the texture coords to those specified by the sprite data
  //! Does not affect shape of mesh
  void setBoxTextureCoords( const pockets::SpriteData &sprite_data );

  //! Set the mesh as a box of sprite's size with correct texture coordinates
  void matchTexture( const pockets::SpriteData &sprite_data );

  //! Transform all vertices by \a mat
  void transform( const ci::Matrix33f &mat );

  //! Make an expanded ribbon from a ci::vec2 skeleton
  //! the skeleton is a collection of points describing the path of the ribbon
  template<typename T>
  void setAsRibbon( const T &skeleton, float width, const ci::vec3 &eyeDir = ci::vec3::zAxis(), bool closed=false );

  //! Make a fat line between two points
  void setAsLine( const ci::vec2 &point_a, const ci::vec2 &point_b, float width=10.0f );
  void setAsCappedLine( const ci::vec2 &point_a, const ci::vec2 &point_b, float width=10.0f );
  void setAsTriangle( const ci::vec3 &a, const ci::vec3 &b, const ci::vec3 &c );

  //! Set the color of all vertices in one go
  void setColor( const ci::ColorA8u &color );
};


template<typename T>
void Shape3D::setAsRibbon( const T &skeleton, float width, const ci::vec3 &eyeDir, bool closed )
{
  using ci::vec3;
  if( vertices.size() != skeleton.size() * 2 )
  { vertices.assign( skeleton.size() * 2, Vertex3D{} ); }
  vec3 a, b, c;
  // first vertex2D
  a = skeleton.at( 0 );
  b = skeleton.at( 1 );
  auto edge = (b - a);
  auto tangent = edge.cross( eyeDir );
  auto north = tangent.normalized() * width;
  vertices.at( 0 ).position = a + north;
  vertices.at( 0 + 1 ).position = a - north;
  // remaining vertices
  for( int i = 1; i < skeleton.size() - 1; ++i )
  {
    a = skeleton.at(i - 1);
    b = skeleton.at(i);
    c = skeleton.at(i + 1);
    edge = ((b - a).normalized() + (c - b).normalized()) * 0.5;
    tangent = edge.cross( eyeDir );
    north = tangent.normalized() * width;
    vertices.at( i * 2 ).position = b + north;
    vertices.at( i * 2 + 1 ).position = b - north;
  }
  // final vertex2D
  c = skeleton.back();
  edge = closed ? (skeleton.at( 1 ) - c) : (c - b);
  tangent = edge.cross( eyeDir );
  north = tangent.normalized() * width;
  size_t end = skeleton.size() - 1;
  vertices.at( end * 2 ).position = c + north;
  vertices.at( end * 2 + 1 ).position = c - north;
}
} // treent::

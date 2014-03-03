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
#include "puptent/PupTent.h"

namespace puptent
{
  struct Vertex
  {
    Vertex( const ci::Vec2f &position = ci::Vec2f::zero(),
            const ci::ColorA8u &color = ci::ColorA8u::white(),
            const ci::Vec2f &uv = ci::Vec2f::zero() ):
    position( position ),
    color( color ),
    tex_coord( uv )
    {}
    ci::Vec2f     position;
    ci::ColorA8u  color;
    ci::Vec2f     tex_coord;
  };
  /**
   RenderMesh:
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
  typedef std::shared_ptr<class RenderMesh> RenderMeshRef;
  class SpriteData;
  struct RenderMesh : Component<RenderMesh>
  {
    RenderMesh( int vertex_count=3 )
    {
      vertices.assign( vertex_count, Vertex{} );
    }
    //! vertices in triangle_strip order
    std::vector<Vertex> vertices;
    //! Convenience method for making circular shapes
    //! If you aren't dynamically changing the circle, consider using a Sprite
    void setAsCircle( const ci::Vec2f &radius, float start_radians=0, float end_radians=M_PI * 2, size_t segments=0 );
    //! Set the mesh bounds to a box shape
    void setAsBox( const ci::Rectf &bounds );
    //! Set the texture coords to those specified by the sprite data
    //! Does not affect shape of mesh
    void setBoxTextureCoords( const SpriteData &sprite_data );
    //! Set the mesh as a box of sprite's size with correct texture coordinates
    void matchTexture( const SpriteData &sprite_data );
    //! Transform all vertices by \a mat
    void transform( const ci::MatrixAffine2f &mat );
    //! Make an expanded ribbon from a ci::Vec2f skeleton
    //! the skeleton is a collection of points describing the path of the ribbon
    template<typename T>
    void setAsRibbon( const T &skeleton, float width, bool closed=false );
    //! Make a fat line between two points
    void setAsLine( const ci::Vec2f &point_a, const ci::Vec2f &point_b, float width=10.0f );
    void setAsCappedLine( const ci::Vec2f &point_a, const ci::Vec2f &point_b, float width=10.0f );
    void setAsTriangle( const ci::Vec2f &a, const ci::Vec2f &b, const ci::Vec2f &c );
    //! Set the color of all vertices in one go
    void setColor( const ci::ColorA8u &color );
  };


  template<typename T>
  void RenderMesh::setAsRibbon( const T &skeleton, float width, bool closed )
  {
    using ci::Vec2f;
    if( vertices.size() != skeleton.size() * 2 )
    { vertices.assign( skeleton.size() * 2, Vertex{} ); }
    Vec2f a, b, c;
    // first vertex
    a = skeleton.at( 0 );
    b = skeleton.at( 1 );
    Vec2f edge = (b - a);
    Vec2f tangent = Vec2f( -edge.y, edge.x );
    Vec2f north = tangent.normalized() * width;
    vertices.at( 0 ).position = a + north;
    vertices.at( 0 + 1 ).position = a - north;
    // remaining vertices
    for( int i = 1; i < skeleton.size() - 1; ++i )
    {
      a = skeleton.at(i - 1);
      b = skeleton.at(i);
      c = skeleton.at(i + 1);
      edge = ((b - a).normalized() + (c - b).normalized()) * 0.5;
      tangent = Vec2f( -edge.y, edge.x );
      north = tangent.normalized() * width;
      vertices.at( i * 2 ).position = b + north;
      vertices.at( i * 2 + 1 ).position = b - north;
    }
    // final vertex
    c = skeleton.back();
    edge = closed ? (skeleton.at( 1 ) - c) : (c - b);
    tangent = Vec2f( -edge.y, edge.x );
    north = tangent.normalized() * width;
    size_t end = skeleton.size() - 1;
    vertices.at( end * 2 ).position = c + north;
    vertices.at( end * 2 + 1 ).position = c - north;
  }
} // puptent::

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
#include "pockets/Pockets.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Rect.h"
#include "cinder/MatrixAffine2.h"
#include <vector>

namespace pockets
{
  struct Vertex
  {
    Vertex( const ci::vec2 &position = ci::vec2( 0 ),
            const ci::ColorA8u &color = ci::ColorA8u::white(),
            const ci::vec2 &uv = ci::vec2( 0 ) ):
    position( position ),
    color( color ),
    tex_coord( uv )
    {}
    ci::vec2     position;
    ci::ColorA8u  color;
    ci::vec2     tex_coord;
  };

  typedef std::shared_ptr<struct RenderMesh2D> RenderMesh2DRef;
  /**
   RenderMesh2D:

   Collection of vertices suitable for rendering.
   Currently, all of the setAs* methods assume a TRIANGLE_STRIP renderer
   Member functions let you conveniently set it to various shapes:
   - Box
   - Circle
   - Ribbon
   - Line
   - Texture billboard (special case of Box)
   Additional methods ease the texturing of those shapes.
  */
  class SpriteData;
  struct RenderMesh2D
  {
    explicit RenderMesh2D( int vertex_count=3 )
    {
      vertices.assign( vertex_count, Vertex() );
    }
    //! vertices in triangle_strip order
    std::vector<Vertex> vertices;
    //! Convenience method for making circular shapes
    //! If you aren't dynamically changing the circle, consider using a Sprite
    void setAsCircle( const ci::vec2 &radius, float start_radians=0, float end_radians=M_PI * 2, size_t segments=0 );
    //! Set the mesh bounds to a box shape
    void setAsBox( const ci::Rectf &bounds );
    //! Transform all vertices by \a mat
    void transform( const ci::mat4 &mat );
    //! Make an expanded ribbon from a ci::vec2 skeleton
    //! the skeleton is a collection of points describing the path of the ribbon
    template<typename T>
    void setAsRibbon( const T &skeleton, float width, bool closed=false );
    //! Make a fat line between two points
    void setAsLine( const ci::vec2 &point_a, const ci::vec2 &point_b, float width=10.0f );
	//! Make a fat line between two points with butt end caps.
    void setAsCappedLine( const ci::vec2 &point_a, const ci::vec2 &point_b, float width=10.0f );
	//! Make a triangle between vertices.
    void setAsTriangle( const ci::vec2 &a, const ci::vec2 &b, const ci::vec2 &c );
    //! Set the color of all vertices in one go (call after setting shape)
    void setColor( const ci::ColorA8u &color );
  };


  template<typename T>
  void RenderMesh2D::setAsRibbon( const T &skeleton, float width, bool closed )
  {
    using ci::vec2;
    using ci::normalize;

    if( vertices.size() != skeleton.size() * 2 )
    { vertices.assign( skeleton.size() * 2, Vertex() ); }
    vec2 a, b, c;
    // first vertex
    a = skeleton.at( 0 );
    b = skeleton.at( 1 );
    vec2 edge = (b - a);
    vec2 tangent = vec2( -edge.y, edge.x );
    vec2 north = normalize( tangent ) * width;
    vertices.at( 0 ).position = a + north;
    vertices.at( 0 + 1 ).position = a - north;
    // remaining vertices
    for( int i = 1; i < skeleton.size() - 1; ++i )
    {
      a = skeleton.at(i - 1);
      b = skeleton.at(i);
      c = skeleton.at(i + 1);
      edge = (normalize( b - a ) + normalize(c - b)) * 0.5;
      tangent = vec2( -edge.y, edge.x );
      north = normalize( tangent ) * width;
      vertices.at( i * 2 ).position = b + north;
      vertices.at( i * 2 + 1 ).position = b - north;
    }
    // final vertex
    c = skeleton.back();
    edge = closed ? (skeleton.at( 1 ) - c) : (c - b);
    tangent = vec2( -edge.y, edge.x );
    north = normalize( tangent ) * width;
    size_t end = skeleton.size() - 1;
    vertices.at( end * 2 ).position = c + north;
    vertices.at( end * 2 + 1 ).position = c - north;
  }
} // puptent::

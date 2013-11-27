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

#include "pockets/rendering/BatchRenderer.h"
#include "pockets/CollectionUtilities.hpp"

using namespace cinder;
using namespace pockets;
using namespace std;

RenderDataRef RenderData::create()
{
  RenderMesh2DRef mesh( new RenderMesh2D() );
  Locus2DRef locus( new Locus2D() );
  return RenderDataRef( new RenderData( mesh, locus ) );
}

void BatchRenderer::append( RenderDataRef data )
{
  int target_layer = data->render_layer;
  if( mGeometry.empty() )
  {
    mGeometry.push_back( data );
  }
  else
  {
    auto iter = mGeometry.begin();
    while( iter != mGeometry.end() && (**iter).render_layer < target_layer )
    { // place the component in the first position on its layer
      ++iter;
    }
    mGeometry.insert( iter, data );
  }
}

void BatchRenderer::remove( RenderDataRef data )
{
  vector_remove( &mGeometry, data );
}

//
//  TRIANGLES
//

void BatchTriangleRenderer::update()
{
  mVertices.clear();
  for( const auto &data : mGeometry )
  {
    auto mesh = data->mesh;
    auto loc = data->locus;
    auto mat = loc->toMatrix();
    for( auto &vert : mesh->vertices )
    {
      mVertices.emplace_back( Vertex{ mat.transformPoint( vert.position ), vert.color, vert.tex_coord } );
    }
  }
}

void BatchTriangleRenderer::draw() const
{
  glEnableClientState( GL_VERTEX_ARRAY );
  glEnableClientState( GL_COLOR_ARRAY );
  glEnableClientState( GL_TEXTURE_COORD_ARRAY );

  glVertexPointer( 2, GL_FLOAT, sizeof( Vertex ), &mVertices[0].position.x );
  glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ), &mVertices[0].tex_coord.x );
  glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex ), &mVertices[0].color.r );
  glDrawArrays( GL_TRIANGLES, 0, mVertices.size() );

  glDisableClientState( GL_VERTEX_ARRAY );
  glDisableClientState( GL_COLOR_ARRAY );
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

//
// TRIANGLE_STRIP
//

void BatchTriangleStripRenderer::update()
{ // assemble vertices for each pass
  mVertices.clear();
  for( const auto &data : mGeometry )
  {
    auto mesh = data->mesh;
    auto loc = data->locus;
    auto mat = loc->toMatrix();
    if( !mVertices.empty() )
    { // create degenerate triangle between previous and current shape
      mVertices.emplace_back( mVertices.back() );
      auto vert = mesh->vertices.front();
      mVertices.emplace_back( Vertex{ mat.transformPoint( vert.position ), vert.color, vert.tex_coord } );
    }
    for( auto &vert : mesh->vertices )
    {
      mVertices.emplace_back( Vertex{ mat.transformPoint( vert.position ), vert.color, vert.tex_coord } );
    }
  }
}

void BatchTriangleStripRenderer::draw() const
{
  glEnableClientState( GL_VERTEX_ARRAY );
  glEnableClientState( GL_COLOR_ARRAY );
  glEnableClientState( GL_TEXTURE_COORD_ARRAY );

  glVertexPointer( 2, GL_FLOAT, sizeof( Vertex ), &mVertices[0].position.x );
  glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ), &mVertices[0].tex_coord.x );
  glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex ), &mVertices[0].color.r );
  glDrawArrays( GL_TRIANGLE_STRIP, 0, mVertices.size() );

  glDisableClientState( GL_VERTEX_ARRAY );
  glDisableClientState( GL_COLOR_ARRAY );
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

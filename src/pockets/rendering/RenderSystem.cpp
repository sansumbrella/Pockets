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

#include "pockets/rendering/RenderSystem.h"
#include "pockets/CollectionUtilities.hpp"
#include "cinder/gl/Texture.h"

using namespace cinder;
using namespace pockets;
using namespace std;

/*
void RenderSystem::receive(const ComponentAddedEvent<puptent::RenderData> &event)
{
  auto data = event.component;
  const RenderPass pass = data->pass;
  if( pass == eNormalPass )
  { // put element in correct sorted position
    int target_layer = data->render_layer;
    if( mGeometry[pass].empty() )
    {
      mGeometry[pass].push_back( event.component );
    }
    else
    {
      auto iter = mGeometry[pass].begin();
      while( iter != mGeometry[pass].end() && (**iter).render_layer < target_layer )
      { // place the component in the first position on its layer
        ++iter;
      }
      mGeometry[pass].insert( iter, data );
    }
  }
  else
  { // just place element at end of list
    // for add and multiply, order doesn't matter
    mGeometry[pass].push_back( event.component );
  }
}
*/

void RenderSystem::checkOrdering() const
{
  if( mGeometry[eNormalPass].size() > 1 )
  {
    for( int i = 0; i < mGeometry[eNormalPass].size() - 2; ++i )
    {
      int lhs = mGeometry[eNormalPass].at( i )->render_layer;
      int rhs = mGeometry[eNormalPass].at( i + 1 )->render_layer;
      if( rhs < lhs )
      {
        std::cout << "ERROR: Render order incorrect: " << rhs << " after " << lhs << std::endl;
      }

    }
  }
}
/*
void RenderSystem::receive(const ComponentRemovedEvent<puptent::RenderData> &event)
{
  auto data = event.component;
  vector_remove( &mGeometry[data->pass], data );
}

void RenderSystem::receive(const EntityDestroyedEvent &event)
{
  auto entity = event.entity;
  auto render_data = entity.component<RenderData>();
  if( render_data )
  { // remove render component from our list
    vector_remove( &mGeometry[render_data->pass], render_data );
  }
}
*/
void RenderSystem::update()
{ // assemble vertices for each pass
  const array<RenderPass, 3> passes = { eNormalPass, eAdditivePass, eMultiplyPass };
  for( const auto &pass : passes )
  {
    auto &v = mVertices[pass];
    v.clear();
    for( const auto &pair : mGeometry[pass] )
    {
      auto mesh = pair->mesh;
      auto loc = pair->locus;
      auto mat = loc->toMatrix();
      if( !v.empty() )
      { // create degenerate triangle between previous and current shape
        v.emplace_back( v.back() );
        auto vert = mesh->vertices.front();
        v.emplace_back( Vertex{ mat.transformPoint( vert.position ), vert.color, vert.tex_coord } );
      }
      for( auto &vert : mesh->vertices )
      {
        v.emplace_back( Vertex{ mat.transformPoint( vert.position ), vert.color, vert.tex_coord } );
      }
    }
  }
}

void RenderSystem::draw() const
{
  float x = 400; // left
  float y = 100; // bottom
  float w = 125;
  float h = 500;
  gl::setViewport( Area( x, y, x + w, y + h ) );
  gl::setMatricesWindow( w, h );
  gl::translate( -x, -y );
//  glMatrixMode( GL_PROJECTION );
//  glFrustum( left, right, bottom, top, near, far );
//  glMatrixMode( GL_MODELVIEW );

  if( mTexture )
  {
    gl::enable( GL_TEXTURE_2D );
    mTexture->bind();
  }
  glEnableClientState( GL_VERTEX_ARRAY );
  glEnableClientState( GL_COLOR_ARRAY );
  glEnableClientState( GL_TEXTURE_COORD_ARRAY );

  // premultiplied alpha blending for normal pass
  gl::enableAlphaBlending( true );
  glVertexPointer( 2, GL_FLOAT, sizeof( Vertex ), &mVertices[eNormalPass][0].position.x );
  glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ), &mVertices[eNormalPass][0].tex_coord.x );
  glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex ), &mVertices[eNormalPass][0].color.r );
  glDrawArrays( GL_TRIANGLE_STRIP, 0, mVertices[eNormalPass].size() );
  // additive blending
  glBlendFunc( GL_SRC_ALPHA, GL_ONE );
  glVertexPointer( 2, GL_FLOAT, sizeof( Vertex ), &mVertices[eAdditivePass][0].position.x );
  glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ), &mVertices[eAdditivePass][0].tex_coord.x );
  glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex ), &mVertices[eAdditivePass][0].color.r );
  glDrawArrays( GL_TRIANGLE_STRIP, 0, mVertices[eAdditivePass].size() );
  // multiply blending
  glBlendFunc( GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA );
  glVertexPointer( 2, GL_FLOAT, sizeof( Vertex ), &mVertices[eMultiplyPass][0].position.x );
  glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex ), &mVertices[eMultiplyPass][0].tex_coord.x );
  glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex ), &mVertices[eMultiplyPass][0].color.r );
  glDrawArrays( GL_TRIANGLE_STRIP, 0, mVertices[eMultiplyPass].size() );

  glDisableClientState( GL_VERTEX_ARRAY );
  glDisableClientState( GL_COLOR_ARRAY );
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  gl::disableAlphaBlending();

  if( mTexture )
  {
    mTexture->unbind();
    gl::disable( GL_TEXTURE_2D );
  }
}

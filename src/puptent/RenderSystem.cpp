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

#include "puptent/RenderSystem.h"
#include "pockets/CollectionUtilities.hpp"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Context.h"

using namespace cinder;
using namespace puptent;
using namespace std;

void RenderSystem::configure( EventManagerRef event_manager )
{
  event_manager->subscribe<EntityDestroyedEvent>( *this );
  event_manager->subscribe<ComponentAddedEvent<RenderData>>( *this );
  event_manager->subscribe<ComponentRemovedEvent<RenderData>>( *this );

  mVbo = gl::Vbo::create( GL_ARRAY_BUFFER, 1.0e5 * sizeof( Vertex ), nullptr, GL_STREAM_DRAW );
  mAttributes = gl::Vao::create();
  gl::VaoScope attr( mAttributes );
  mVbo->bind();
  mRenderProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( app::loadAsset( "renderer.vs" ) )
                                     .fragment( app::loadAsset( "renderer.fs" ) )
                                     .attribLocation( "iPosition", 0 )
                                     .attribLocation( "iColor", 1 )
                                     .attribLocation( "iTexCoord", 2 ) );
  gl::enableVertexAttribArray( 0 );
  gl::enableVertexAttribArray( 1 );
  gl::enableVertexAttribArray( 2 );

  gl::vertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, position) );
  gl::vertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, color));
  gl::vertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex,tex_coord) );
  mVbo->unbind();
}

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

void RenderSystem::update( EntityManagerRef es, EventManagerRef events, double dt )
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
  Vertex *ptr = static_cast<Vertex*>( mVbo->map( GL_WRITE_ONLY ) );
  for( const auto &pass : passes )
  {
    for( const auto &vertex : mVertices[pass] )
    {
      *ptr = vertex;
      ++ptr;
    }
  }
  mVbo->unmap();
}

void RenderSystem::draw() const
{
  gl::VaoScope attr( mAttributes );
  gl::GlslProgScope shader( mRenderProg );
  gl::setDefaultShaderVars();
  gl::TextureBindScope( mTexture, 0 );

  // premultiplied alpha blending for normal pass
  gl::enableAlphaBlending( true );

  size_t begin = 0;
  size_t count = mVertices[eNormalPass].size();
  gl::drawArrays( GL_TRIANGLE_STRIP, begin, count );

  // additive blending
  begin += count;
  count = mVertices[eAdditivePass].size();
//  glBlendFunc( GL_SRC_ALPHA, GL_ONE );
//  gl::drawArrays( GL_TRIANGLE_STRIP, begin, count );
//  // multiply blending
//  begin += count;
//  count = mVertices[eMultiplyPass].size();
//  glBlendFunc( GL_DST_COLOR,  GL_ONE_MINUS_SRC_ALPHA );
//  gl::drawArrays( GL_TRIANGLE_STRIP, begin, count );
  gl::disableAlphaBlending();
}

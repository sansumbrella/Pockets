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

#include "treant/ShapeRenderSystem.h"
#include "pockets/CollectionUtilities.hpp"
#include "treant/LocationComponent.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Context.h"
#include "cinder/app/App.h"

using namespace std;
using namespace cinder;
using namespace pockets;

namespace treant
{

std::string defaultFragment()
{
return R"(
#version 150 core

in vec4 Color;

out vec4 oColor;

void main()
{
  oColor = Color;
}
)";
}

std::string defaultVertex()
{
return R"(
#version 150 core

uniform mat4 ciModelViewProjection;

in vec2 iPosition;
in vec4 iColor;
in vec2 iTexCoord;

out vec4 Color;
out vec2 TexCoord;

void main()
{
  Color = iColor;
  TexCoord = iTexCoord;
  gl_Position = ciModelViewProjection * vec4( iPosition, 0.0, 1.0 );
}
)";
}

void ShapeRenderSystem::configure( EventManagerRef event_manager )
{
  event_manager->subscribe<EntityDestroyedEvent>( *this );
  event_manager->subscribe<ComponentAddedEvent<RenderData>>( *this );
  event_manager->subscribe<ComponentRemovedEvent<RenderData>>( *this );

  // make buffer large enough to hold all the vertices you will ever need
  mVbo = gl::Vbo::create( GL_ARRAY_BUFFER, 1.0e5 * sizeof( Vertex2D ), nullptr, GL_STREAM_DRAW );
  mAttributes = gl::Vao::create();
  gl::ScopedVao attr( mAttributes );
  mVbo->bind();
  mRenderProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( defaultVertex().c_str() )
                                     .fragment( defaultFragment().c_str() )
                                     .attribLocation( "iPosition", 0 )
                                     .attribLocation( "iColor", 1 )
                                     .attribLocation( "iTexCoord", 2 ) );
  gl::enableVertexAttribArray( 0 );
  gl::enableVertexAttribArray( 1 );
  gl::enableVertexAttribArray( 2 );

  gl::vertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const GLvoid*)offsetof(Vertex2D, position) );
  gl::vertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const GLvoid*)offsetof(Vertex2D, color));
  gl::vertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const GLvoid*)offsetof(Vertex2D,tex_coord) );
  mVbo->unbind();
}

void ShapeRenderSystem::receive(const ComponentAddedEvent<RenderData> &event)
{
  auto data = event.component;
  const RenderPass pass = data->pass;
  if( pass == PREMULTIPLIED )
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

void ShapeRenderSystem::checkOrdering() const
{
  if( mGeometry[PREMULTIPLIED].size() > 1 )
  {
    for( int i = 0; i < mGeometry[PREMULTIPLIED].size() - 2; ++i )
    {
      int lhs = mGeometry[PREMULTIPLIED].at( i )->render_layer;
      int rhs = mGeometry[PREMULTIPLIED].at( i + 1 )->render_layer;
      if( rhs < lhs )
      {
        std::cout << "ERROR: Render order incorrect: " << rhs << " after " << lhs << std::endl;
      }

    }
  }
}

void ShapeRenderSystem::receive(const ComponentRemovedEvent<RenderData> &event)
{
  auto render_data = event.component;
  vector_remove( &mGeometry[render_data->pass], render_data );
}

void ShapeRenderSystem::receive(const EntityDestroyedEvent &event)
{
  auto entity = event.entity;
  auto render_data = entity.component<RenderData>();
  if( render_data )
  { // remove render component from our list
    vector_remove( &mGeometry[render_data->pass], render_data );
  }
}

namespace {
  const array<RenderPass, NUM_RENDER_PASSES> passes = { PREMULTIPLIED, ADD, MULTIPLY };
} // anon::

void ShapeRenderSystem::update( EntityManagerRef es, EventManagerRef events, double dt )
{ // assemble vertices for each pass
  for( const auto &pass : passes )
  {
    auto &v = mVertices[pass];
    v.clear();
    for( const auto &pair : mGeometry[pass] )
    {
      auto mesh = pair->mesh;
      auto loc = pair->locus;
      auto mat = loc->matrix;
      if( !v.empty() ) {
        // create degenerate triangle between previous and current shape
        v.push_back( v.back() );
        auto vert = mesh->vertices.front();
        v.emplace_back( Vertex2D{ mat.transformPoint( vert.position ), vert.color, vert.tex_coord } );
      }
      for( auto &vert : mesh->vertices ) {
        v.emplace_back( Vertex2D{ mat.transformPoint( vert.position ), vert.color, vert.tex_coord } );
      }
    }
  }

  GLintptr  offset = 0;
  for( const auto &pass : passes )
  {
    if( !mVertices[pass].empty() ) {
      mVbo->bufferSubData( offset, mVertices[pass].size() * sizeof( Vertex2D ), mVertices[pass].data() );
      offset += mVertices[pass].size() * sizeof( Vertex2D );
    }
  }
}

void ShapeRenderSystem::draw() const
{
  gl::ScopedGlslProg    shader( mRenderProg );
  gl::ScopedVao         attr( mAttributes );

  gl::setDefaultShaderVars();

  // premultiplied alpha blending for normal pass
  gl::ScopedBlend premultBlend( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

  size_t begin = 0;
  size_t count = mVertices[PREMULTIPLIED].size();
  gl::drawArrays( GL_TRIANGLE_STRIP, begin, count );

  // additive blending
  begin += count;
  count = mVertices[ADD].size();
  gl::ScopedBlend addBlend( GL_SRC_ALPHA, GL_ONE );
  gl::drawArrays( GL_TRIANGLE_STRIP, begin, count );

  // multiply blending
  begin += count;
  count = mVertices[MULTIPLY].size();
  gl::ScopedBlend multBlend( GL_DST_COLOR,  GL_ONE_MINUS_SRC_ALPHA );
  gl::drawArrays( GL_TRIANGLE_STRIP, begin, count );
  gl::disableAlphaBlending();

}

}

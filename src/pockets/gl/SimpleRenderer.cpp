/*
 * Copyright (c) 2013 David Wicks
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

#include "SpriteRenderer.h"

using namespace std;
using namespace ci;
using namespace pockets;

SpriteRenderer::SpriteRenderer()
{}

SpriteRenderer::~SpriteRenderer()
{}

void SpriteRenderer::add( const Geometry &geo, int layer )
{
  mGeometry.push_back( geo );
}

void SpriteRenderer::draw()
{
  vector<Vec2f> positions;
  vector<Vec2f> texcoords;
  int layer = baseLayer();

  mSpriteSheet->enableAndBind();

  for( const Geometry &geo : mGeometry )
  {
    positions.insert( positions.end(), geo.positions.begin(), geo.positions.end() );
    texcoords.insert( texcoords.end(), geo.texcoords.begin(), geo.texcoords.end() );
    if( geo.layer != layer )
    { // send to GPU
      render( positions, texcoords );
      positions.clear();
      texcoords.clear();
      layer = geo.layer;
    }
  }

  render( positions, texcoords );
  mSpriteSheet->unbind();
}

void SpriteRenderer::render(const std::vector<ci::Vec2f> &positions, const std::vector<ci::Vec2f> &texcoords)
{
  glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, &positions[0] );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glTexCoordPointer( 2, GL_FLOAT, 0, &texcoords[0] );

	glDrawArrays( GL_TRIANGLES, 0, positions.size() );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

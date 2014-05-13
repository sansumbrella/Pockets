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

#include "Renderer2d.h"
#include "CollectionUtilities.hpp"

using namespace std;
using namespace cinder;
using namespace pockets;

BatchRenderer2d::Renderable::~Renderable()
{
	clearHosts();
}

void BatchRenderer2d::Renderable::clearHosts()
{
	auto hosts = mHosts;
	for( auto host : hosts )
	{
		host->remove( this );
	}
}

BatchRenderer2d::Renderable::Renderable( const Renderable &other ):
mLayer( other.mLayer )
{
	clearHosts();
	for( auto &host : other.mHosts )
	{
		host->add( this );
	}
}

BatchRenderer2d::Renderable& BatchRenderer2d::Renderable::operator = (const BatchRenderer2d::Renderable &rhs)
{
	clearHosts();
	for( auto &host : rhs.mHosts )
	{
		host->add( this );
	}
	mLayer = rhs.mLayer;
	return *this;
}

BatchRenderer2d::~BatchRenderer2d()
{
	for( auto child : mRenderables )
	{
    vector_remove( &child->mHosts, this );
	}
}

void BatchRenderer2d::add( Renderable *renderable )
{
	mRenderables.push_back( renderable );
	renderable->mHosts.push_back( this );
}

void BatchRenderer2d::remove( Renderable *renderable )
{
	vector_remove( &mRenderables, renderable );
	vector_remove( &renderable->mHosts, this );
}

void BatchRenderer2d::sort( const SortFn &fn )
{
	stable_sort( mRenderables.begin(), mRenderables.end(), fn );
}

bool BatchRenderer2d::sortByLayerAscending( const Renderable *lhs, const Renderable *rhs )
{
	return lhs->getLayer() < rhs->getLayer();
}

void Renderer2dStrip::update()
{
	mVertices.clear();
// assemble all vertices
	for( auto &r : renderables() )
	{
		auto v = r->getVertices();
		if( !mVertices.empty() )
		{	// insert a degenerate triangle to bridge space
			mVertices.emplace_back( mVertices.back() );
			mVertices.emplace_back( v.front() );
		}
		mVertices.insert( mVertices.end(), v.begin(), v.end() );
	}
}

void Renderer2dStrip::render()
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer( 2, GL_FLOAT, sizeof( Vertex2d ), &mVertices[0].position.x );
	glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex2d ), &mVertices[0].tex_coord.x );
	glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex2d ), &mVertices[0].color.r );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, mVertices.size() );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

void Renderer2dStripVbo::update()
{
	vector<Vertex2d> vertices;
// assemble all vertices
	vector<Vertex2d> pv;
	for( auto &r : renderables() )
	{
		auto v = r->getVertices();
		if( !pv.empty() )
		{	// insert a degenerate triangle to bridge space
			vertices.emplace_back( pv.back() );
			vertices.emplace_back( v.front() );
		}
		vertices.insert( vertices.end(), v.begin(), v.end() );
		pv = move(v);
	}
	if( !mVBO )
  {
    glGenBuffers( 1, &mVBO );
	  glBindBuffer( GL_ARRAY_BUFFER, mVBO );
	  glBufferData( GL_ARRAY_BUFFER, sizeof( Vertex2d ) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW );
  }
  else
  {
  	glBindBuffer( GL_ARRAY_BUFFER, mVBO );
  	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( Vertex2d ) * vertices.size(), &vertices[0] );
  }
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  mSize = vertices.size();
}

void Renderer2dStripVbo::render()
{
	if( mVBO )
	{
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_COLOR_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );

		const auto color_offset = sizeof( Vec2f );
		const auto tex_coord_offset = color_offset + sizeof( ColorA8u );
		glBindBuffer( GL_ARRAY_BUFFER, mVBO );
		glVertexPointer( 2, GL_FLOAT, sizeof( Vertex2d ), 0 );
		glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex2d ), (GLvoid*)color_offset );
		glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex2d ), (GLvoid*)tex_coord_offset );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, mSize );

		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}
}

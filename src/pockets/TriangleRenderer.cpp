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

#include "TriangleRenderer.h"
#include "CollectionUtilities.hpp"

using namespace std;
using namespace cinder;
using namespace pockets;

TriangleRenderer::IRenderable::~IRenderable()
{
	if( mHost )
	{
		mHost->remove( this );
	}
}

TriangleRenderer::IRenderable::IRenderable( const IRenderable &other ):
mLayer( other.mLayer )
{
	if( other.mHost )
	{
		other.mHost->add( this );
	}
}

TriangleRenderer::~TriangleRenderer()
{
	for( auto child : mRenderables )
	{
		child->mHost = nullptr;
	}
}

void TriangleRenderer::add( IRenderable *renderable )
{
	if( renderable->mHost )
	{
		renderable->mHost->remove( renderable );
	}
	mRenderables.push_back( renderable );
	renderable->mHost = this;
}

void TriangleRenderer::remove( IRenderable *renderable )
{
	vector_remove( &mRenderables, renderable );
	renderable->mHost = nullptr;
}

void TriangleRenderer::sort( const TriangleRenderer::SortFn &fn )
{
	stable_sort( mRenderables.begin(), mRenderables.end(), fn );
}

bool TriangleRenderer::sortByLayerAscending( const IRenderable *lhs, const IRenderable *rhs )
{
	return lhs->getLayer() < rhs->getLayer();
}

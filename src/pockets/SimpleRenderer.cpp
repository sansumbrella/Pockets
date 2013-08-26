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

#include "SimpleRenderer.h"
#include "CollectionUtilities.hpp"

using namespace std;
using namespace ci;
using namespace pockets;

SimpleRenderer::IRenderable::~IRenderable()
{
  if( mHost )
  { mHost->remove( this ); }
}

SimpleRenderer::IRenderable::IRenderable( const SimpleRenderer::IRenderable &other ):
mLayer( other.mLayer )
{
  if( other.mHost )
  { other.mHost->add( this ); }
}

SimpleRenderer::IRenderable& SimpleRenderer::IRenderable::operator = (const pockets::SimpleRenderer::IRenderable &rhs)
{
  if( mHost == nullptr && rhs.mHost )
  { rhs.mHost->add( this ); }
  mLayer = rhs.mLayer;
  return *this;
}

SimpleRenderer::SimpleRenderer()
{}

SimpleRenderer::~SimpleRenderer()
{
  for( auto child : mRenderables )
  {
    child->mHost = nullptr;
  }
}

void SimpleRenderer::add( IRenderable *renderable )
{
  if( renderable->mHost )
  { renderable->mHost->remove( renderable ); }
	mRenderables.push_back( renderable );
  renderable->mHost = this;
}

void SimpleRenderer::remove( IRenderable *renderable )
{
  vector_remove( &mRenderables, renderable );
  renderable->mHost = nullptr;
}


void SimpleRenderer::render()
{
  if( mPreDraw ){ mPreDraw(); }

  for( const auto &obj : mRenderables )
  {
    obj->render();
  }

  if( mPostDraw ){ mPostDraw(); }
}

void SimpleRenderer::sort( const SortFn &fn )
{
  stable_sort( mRenderables.begin(), mRenderables.end(), fn );
}

bool SimpleRenderer::sortByLayerAscending(const IRenderable *lhs, const IRenderable *rhs)
{
  return lhs->getLayer() < rhs->getLayer();
}

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
#include "Pockets.h"

/**
 A basic renderer for grouping rendered content and rendering in order.

 The IRenderable object's destructor removes it from the renderer, which keeps a
 non-owning raw pointer to the object, allowing you to manage the object's
 lifetime how you see fit.

 For now, this is a 2d triangle renderer. I will templatize it to be either 2 or 3

 */

class TriangleRenderer
{
public:
  class IRenderable
  {
    IRenderable() = default;
    IRenderable( const IRenderable &other );
    virtual ~IRenderable();
    //! return positions as for a TRIANGLE_STRIP
    virtual std::vector<ci::Vec2f>  getPositions() = 0;
    //! return texture coordinates as for a TRIANGLE_STRIP
    virtual std::vector<ci::Vec2f>  getTexCoords() = 0;
    void setLayer(int layer){ mLayer = layer; }
    int getLayer() const { return mLayer; }
  private:
    friend class TriangleRenderer;
    TriangleRenderer *mHost;
    int               mLayer = 0;
  };
  typedef std::function<bool (const IRenderable*, const IRenderable*)> SortFn;
  TriangleRenderer() = default;
  ~TriangleRenderer();

  void add( IRenderable *renderable );
  void remove( IRenderable *renderable );
  void sort( const SortFn &fn = sortByLayerAscending );
  void render();
  static bool sortByLayerAscending( const IRenderable *lhs, const IRenderable *rhs );
private:
  std::vector<IRenderable*> mRenderables;
};

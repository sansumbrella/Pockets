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

#pragma once

#include "Pockets.h"

/**
A basic renderer for grouping rendered content and rendering in order.
Defaults to sorting content by layer before rendering.
Pre- and post- draw hooks let you set up the proper render context.

Note that SimpleRenderer only accepts pointers to IRenderable elements.
You will need to manage the lifetime of your renderable objects yourself.

When IRenderable objects destruct, they automatically remove themselves from
the SimpleRenderer.
*/
namespace pockets
{

typedef std::shared_ptr<class SimpleRenderer> SimpleRendererRef;
class SimpleRenderer
{
public:
  class IRenderable
  {
  public:
    IRenderable() = default;
    virtual ~IRenderable();
    virtual void render() = 0;
    //! set the object layer to affect render order (higher => later)
    void setLayer( int layer ){ mLayer = layer; }
    int getLayer() const { return mLayer; }
  private:
    friend class SimpleRenderer;
    SimpleRenderer*  mHost = nullptr;
    int              mLayer = 0;
  };

  typedef std::function<void ()>  PrepFn;
  typedef std::function<bool (const IRenderable*, const IRenderable*)>  SortFn;

  SimpleRenderer();
  ~SimpleRenderer();
  //! add an element to be rendered
  void add( IRenderable *renderable );
  //! remove an element from the renderer
  void remove( IRenderable *renderable );
  //! sorts the renderable contents by layer (or by custom method if provided)
  //! only needs to be called if the draw order is changing (or after adding new content)
  void update();
  //! draw everything in sorted order
  void draw();
  //! set function to call before rendering
  void setPreDrawFn( const PrepFn &fn ){ mPreDraw = fn; }
  //! set function to call after rendering
  void setPostDrawFn( const PrepFn &fn ){ mPostDraw = fn; }
  //! set function for sorting elements to be drawn
  void setSortFn( const SortFn &fn ){ mSortFn = fn; }

  static SimpleRendererRef create(){ return SimpleRendererRef( new SimpleRenderer ); }
private:
  std::vector<IRenderable*> mRenderables;
  PrepFn  mPreDraw;
  PrepFn  mPostDraw;
  SortFn  mSortFn = &SimpleRenderer::defaultSort;

  static bool defaultSort( const IRenderable *lhs, const IRenderable *rhs );
};

} // pockets::
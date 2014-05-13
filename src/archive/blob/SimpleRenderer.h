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
Pre- and post- draw hooks let you set up the proper render context.

Renders anything with a render() method (overridden from Renderable)

Renderable objects' destructors remove them from the renderer, which keeps a
non-owning raw pointer to renderables, allowing you to manage the object's
lifetime as you see fit. Symmetrically, the renderer will disown any renderables
when it is destructed.

The copy-constructor and copy-assignment operators are designed to preserve the
expected behavior in case you have a std::vector of renderables.
 */
namespace pockets
{

typedef std::shared_ptr<class SimpleRenderer> SimpleRendererRef;
class SimpleRenderer
{
public:
  class Renderable
  { // a renderable type; removes itself from the parent renderer on destruction
  public:
    Renderable() = default;
    Renderable( const Renderable &other );
    Renderable& operator=(const Renderable &rhs);
    virtual ~Renderable();
    virtual void render() = 0;
    //! set the object layer to affect render order (higher => later)
    void setLayer( int layer ){ mLayer = layer; }
    int getLayer() const { return mLayer; }
  private:
    friend class SimpleRenderer;
    SimpleRenderer*  mHost = nullptr;
    int              mLayer = 0;
  };

  typedef std::function<void ()>  ScaffoldFn;
  typedef std::function<bool (const Renderable*, const Renderable*)>  SortFn;

  SimpleRenderer();
  ~SimpleRenderer();
  //! add an element to be rendered
  void add( Renderable *renderable );
  //! remove an element from the renderer
  void remove( Renderable *renderable );
  //! sorts the renderable contents by layer (or by custom method if provided)
  //! only needs to be called if the draw order is changing (or after adding new content)
  void sort( const SortFn &fn = sortByLayerAscending );
  //! draw everything in sorted order
  void render();
  //! set function to call before rendering
  void setPreDrawFn( const ScaffoldFn &fn ){ mPreDraw = fn; }
  //! set function to call after rendering
  void setPostDrawFn( const ScaffoldFn &fn ){ mPostDraw = fn; }

  static SimpleRendererRef create(){ return SimpleRendererRef( new SimpleRenderer ); }
  static bool sortByLayerAscending( const Renderable *lhs, const Renderable *rhs );
private:
  std::vector<Renderable*> mRenderables;
  ScaffoldFn  mPreDraw;
  ScaffoldFn  mPostDraw;
};

} // pockets::
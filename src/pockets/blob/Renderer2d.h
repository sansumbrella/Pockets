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
#include "Types.h"

/**
 A basic renderer for grouping rendered content and rendering in order.

 Renders anything that produces triangle vertices (overridden from Renderable)
 Vertices are assumed to be in a format that works with GL_TRIANGLE_STRIP

 Renderable objects' destructors remove them from the renderer, which keeps a
 non-owning raw pointer to renderables, allowing you to manage the object's
 lifetime as you see fit. Symmetrically, the renderer will disown any renderables
 when it is destructed.

 For now, this renders 2D Triangle Strips.
 Things under consideration:
 3D version
 GL_TRIANGLES version
 */

namespace pockets
{
  /**
   Base class for 2d renderers
   Handles sorting of renderable elements by layer.
   Stores a collection of renderable elements.

   Renderables and Renderer2d manage their circular relationship automatically
   in their destructors.
   */

  class BatchRenderer2d;
  typedef std::shared_ptr<class Locus2d>  Locus2dRef;

  class SpriteBatcher2d
  {

  };

  class BatchRenderer2d
  {
  public:
    /**
     Interface that all elements to be rendered must implement
     Most important method is getVertices()
     */
    class Renderable
    {
    public:
      //! returns vertices at final render position
      //! override to provide vertex data to renderer
      virtual std::vector<Vertex2d> getVertices() const = 0;
      // Constructors and destructor
      Renderable() = default;
      Renderable( const Renderable &other );
      Renderable& operator = ( const Renderable &rhs );
      virtual ~Renderable();
      inline int getLayer() const { return mLayer; }
      inline void setLayer( int layer ){ mLayer = layer; }
    private:
      friend class BatchRenderer2d;
      int mLayer = 0;
      std::vector<BatchRenderer2d*> mHosts;
      void clearHosts();
    };

    BatchRenderer2d() = default;
    virtual ~BatchRenderer2d();
    typedef std::function<bool (const Renderable*, const Renderable*)> SortFn;
    void add( Renderable *renderable );
    void remove( Renderable *renderable );
    //! Arrange renderable elements
    void sort( const SortFn &fn = sortByLayerAscending );
    //! Collect vertices for rendering
    virtual void update() = 0;
    //! Send vertices to GPU to render
    virtual void render() = 0;
    const std::vector<Renderable*> &renderables() const { return mRenderables; }
  private:
    static bool sortByLayerAscending( const Renderable *lhs, const Renderable *rhs );
    std::vector<Renderable*>         mRenderables;
  };

  /**
   Batch renderer for GL_TRIANGLE_STRIP geometry
   Stores geometry on the CPU and uploads on render
   */
  class Renderer2dStrip : public BatchRenderer2d
  {
  public:
    //! get all vertices and store in mVertices
    void    update() override;
    //! render vertices (fast)
    void    render() override;
  private:
    std::vector<Vertex2d>  mVertices;
  };

  /**
   Batch renderer for GL_TRIANGLE_STRIP geometry
   Stores geometry in a VBO on the GPU
   */
  class Renderer2dStripVbo : public BatchRenderer2d
  {
  public:
    //! get all vertices and update vbo
    void    update() override;
    //! render vbo (really fast)
    void    render() override;
  private:
    GLuint  mVBO = 0;
    size_t  mSize = 0;
  };

} // pockets::

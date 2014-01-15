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

#include "pockets/Pockets.h"
#include "pockets/Locus.h"
#include "pockets/RenderMesh.h"
#include <array>

namespace pockets
{
  /**
   RenderData:
 
   Composite data
   The mesh (vertices) and locus (transform) are stored here.
   Also, we store a render layer to support layer-sorted 2d rendering
   */
  typedef std::shared_ptr<class RenderData> RenderDataRef;
  struct RenderData
  {
    RenderData( RenderMesh2DRef mesh, Locus2DRef locus, int render_layer=0 ):
    mesh( mesh ),
    locus( locus ),
    render_layer( render_layer )
    {}
    static RenderDataRef create();
    RenderMesh2DRef   mesh;
    Locus2DRef        locus;
    int               render_layer;
    static bool       layerSort( const RenderDataRef &lhs, const RenderDataRef &rhs )
    { return lhs->render_layer < rhs->render_layer; }
  };

  /**
   BatchRenderer:

   Single-pass batch renderer.

   Each pass is a batch of RenderData components combined into a single
   triangle strip. The render passes are:
   1. Normal Pass
   - Geometry is drawn in layer order with premultiplied alpha blending
   2. Additive Pass
   - Geometry is drawn with additive blending (color += color.rgb)
   3. Multiply Pass
   - Geometry is drawn with multiply blending (color *= color.rgb)
   If a texture is assigned, it will be bound before rendering begins.
   The same texture will remain bound through all render passes.
   For "untextured" geometry, we leave a white pixel in the top-left corner
   of our sprite sheets and set all vertex tex coords to their default 0,0.
   */
  typedef std::shared_ptr<class BatchRenderer>  BatchRendererRef;
  class BatchRenderer
  {
  public:
    virtual ~BatchRenderer() {}
    //! sort the render data in the normal pass by render layer
    //! needed iff you are dynamically changing render_layers
    inline void sort()
    { stable_sort( mGeometry.begin(), mGeometry.end(), &RenderData::layerSort ); }
    //! add a piece of RenderData to draw
    void        append( RenderDataRef data );
    //! remove a piece of RenderData
    void        remove( RenderDataRef data );
    //! generate vertex list by transforming meshes by locii
    virtual void update() = 0;
    //! batch render scene to screen
    virtual void draw() const = 0;
  protected:
    std::vector<RenderDataRef>	mGeometry;
    std::vector<Vertex>         mVertices;
  };

  class BatchTriangleRenderer : public BatchRenderer
  {
  public:
    void update();
    void draw() const;
  };

  class BatchTriangleStripRenderer : public BatchRenderer
  {
  public:
    void update();
    void draw() const;
    static std::shared_ptr<BatchTriangleStripRenderer> create()
    { return std::shared_ptr<BatchTriangleStripRenderer>( new BatchTriangleStripRenderer() ); }
  };

} // pockets::

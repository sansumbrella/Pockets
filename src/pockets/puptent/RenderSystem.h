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

#include "pockets/puptent/PupTent.h"
#include "pockets/puptent/Locus.h"
#include "pockets/puptent/RenderMesh.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Vbo.h"

namespace pockets
{ namespace puptent
  {
    /**
     RenderPass:
     Flag to tell RenderSystem when to draw the given geometry.
     First, the normal pass is drawn in layer-sorted order.
     Second, an additive pass is made (unsorted, since it doesn't affect output).
     Finally, a multiplicative pass is made (also unsorted).
     */
    enum RenderPass
    {
      PREMULTIPLIED,
      ADD,
      MULTIPLY,
      NUM_RENDER_PASSES
    };

    /**
     RenderData:
     Composite component.
     Lets us store information needed for RenderSystem in one fast-to-access place.
     Requires an extra step when defining element components
     */
    typedef std::shared_ptr<class RenderData> RenderDataRef;
    struct RenderData : Component<RenderData>
    {
      RenderData( RenderMeshRef mesh, LocusRef locus, int render_layer=0, RenderPass pass=PREMULTIPLIED ):
      mesh( mesh ),
      locus( locus ),
      render_layer( render_layer ),
      pass( pass )
      {}
      RenderMeshRef     mesh;
      LocusRef          locus;
      int               render_layer;
      const RenderPass  pass;
    };

    /**
     RenderSystem:
     Multi-pass, layer-sorted rendering system.

     TODO:
     New RenderSystem type (combined 2d and 3d)
     Separate passes for blend mode only.
     Determine layer-order by using OpenGL depth buffer; should enable more seamless 2d + 3d combinations.
     Provide convenience method for 1:1 scaling when pushing content back in Z.
     Use render pass like tag predicate for iteration.
     Skip the initial render pass generation.
     For sprites, we can't draw to depth buffer (because of masking in zero-alpha regions)

     The RenderSystem is designed to quickly display active entities. It can
     handle all of your sprites, particles, and generative 2d meshes.

     For rendering large background and foreground elements, use a different system.

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
    struct RenderSystem : public System<RenderSystem>, Receiver<RenderSystem>
    {
      //! listen for events
      void        configure( EventManagerRef event_manager ) override;
      //! generate vertex list by transforming meshes by locii
      void        update( EntityManagerRef es, EventManagerRef events, double dt ) override;
      //! batch render scene to screen
      void        draw() const;
      //! set a texture to be bound for all rendering
      inline void setTexture( ci::gl::TextureRef texture )
      { mTexture = texture; }
      void        receive( const EntityDestroyedEvent &event );
      void        receive( const ComponentAddedEvent<RenderData> &event );
      void        receive( const ComponentRemovedEvent<RenderData> &event );
      void        checkOrdering() const;
      //! sort the render data in the normal pass by render layer
      //! needed iff you are dynamically changing Locus render_layers
      inline void sort()
      { stable_sort( mGeometry[PREMULTIPLIED].begin(), mGeometry[PREMULTIPLIED].end(), &RenderSystem::layerSort ); }
    private:
      std::array<std::vector<RenderDataRef>, NUM_RENDER_PASSES> mGeometry;
      std::array<std::vector<Vertex>, NUM_RENDER_PASSES>        mVertices;
      ci::gl::VboRef                            mVbo;
      ci::gl::VaoRef                            mAttributes;
      ci::gl::TextureRef                        mTexture;
      ci::gl::GlslProgRef                       mRenderProg;
      static bool                 layerSort( const RenderDataRef &lhs, const RenderDataRef &rhs )
      { return lhs->render_layer < rhs->render_layer; }
      // maybe add a CameraRef for positioning the scene
      // use a POV and Locus component as camera, allowing dynamic switching
    };
    
  } // puptent::
} // pockets::

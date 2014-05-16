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

#include "treent/Treent.h"
#include "treent/ShapeComponent.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Vbo.h"

namespace treent
{

typedef std::shared_ptr<struct ShapeComponent>      ShapeComponentRef;
typedef std::shared_ptr<struct LocationComponent>   LocationComponentRef;

typedef std::shared_ptr<struct LayeredShapeRenderData> LayeredShapeRenderDataRef;
/**
 LayeredShapeRenderData:
 Composite component.
 Lets us store information needed for LayeredShapeRenderSystem in one fast-to-access place.
 Requires an extra step when defining element components
 */
struct LayeredShapeRenderData : Component<LayeredShapeRenderData>
{
  LayeredShapeRenderData( ShapeComponentRef mesh, LocationComponentRef locus, int render_layer=0 ):
  mesh( mesh ),
  locus( locus ),
  render_layer( render_layer )
  {}
  ShapeComponentRef     mesh;
  LocationComponentRef  locus;
  int                   render_layer;
};

/**
 LayeredShapeRenderSystem:
 Multi-pass, layer-sorted rendering system.

 TODO:
 New RenderSystem type (combined 2d and 3d)
 Determine layer-order by using OpenGL depth buffer for non-sprited content; should enable more seamless 2d + 3d combinations.
 Provide convenience method for 1:1 scaling when pushing content back in Z.
 Use render pass like tag predicate for iteration.
 Skip the initial render pass generation.
 For sprites, we can't draw to depth buffer (because of masking in zero-alpha regions)

 The LayeredShapeRenderSystem is designed to quickly display active entities. It can
 handle all of your sprites, particles, and generative 2d meshes.

 For rendering large background and foreground elements, use a different system.

 Each pass is a batch of LayeredShapeRenderData components combined into a single
 triangle strip. The render passes are:
 1. Normal Pass
 - Geometry is drawn in layer order with premultiplied alpha blending
 If a texture is assigned, it will be bound before rendering begins.
 The same texture will remain bound through all render passes.
 For "untextured" geometry, we leave a white pixel in the top-left corner
 of our sprite sheets and set all vertex tex coords to their default 0,0.
 */
class LayeredShapeRenderSystem : public System<LayeredShapeRenderSystem>, public Receiver<LayeredShapeRenderSystem>
{
public:
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
  void        receive( const ComponentAddedEvent<LayeredShapeRenderData> &event );
  void        receive( const ComponentRemovedEvent<LayeredShapeRenderData> &event );
  void        checkOrdering() const;
  //! sort the render data in the normal pass by render layer
  //! needed iff you are dynamically changing Locus render_layers
  inline void sort()
  { stable_sort( mGeometry.begin(), mGeometry.end(), &LayeredShapeRenderSystem::layerSort ); }
private:
  std::vector<LayeredShapeRenderDataRef>    mGeometry;
  std::vector<Vertex2D>         mVertices;
  ci::gl::VboRef                mVbo;
  ci::gl::VaoRef                mAttributes;
  ci::gl::TextureRef            mTexture;
  ci::gl::GlslProgRef           mRenderProg;
  static bool                 layerSort( const LayeredShapeRenderDataRef &lhs, const LayeredShapeRenderDataRef &rhs )
  { return lhs->render_layer < rhs->render_layer; }
  // maybe add a CameraRef for positioning the scene
  // use a POV and Locus component as camera, allowing dynamic switching
};

} // treent::

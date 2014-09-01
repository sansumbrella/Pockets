/*
 * Copyright (c) 2014 David Wicks, sansumbrella.com
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

namespace treent
{

typedef std::shared_ptr<struct GuiComponent>  GuiComponentRef;
static const uint32_t MOUSE_ID = std::numeric_limits<uint32_t>::max();

/**
 GuiComponents handle user interaction events as they pass through the tree.
 They have an interaction bounds area.

 They are unlike other components in that they are updated during event propagation,
 which moves through the tree. Since they don't know anything about the other components
 attached to

 Provides no-op default implementations of interaction events.
 Handlers should return true to indicate they handled the event and should stop propagation.
 In general, only begin/end events will be captured by a single object.
 */
struct GuiComponent : treent::Component<GuiComponent>
{
  virtual ~GuiComponent() = default;
  virtual bool    touchesBegan( ci::app::TouchEvent &event, const ci::mat4 &world_transform ) { return false; }
  virtual bool    touchesMoved( ci::app::TouchEvent &event, const ci::mat4 &world_transform ) { return false; }
  virtual bool    touchesEnded( ci::app::TouchEvent &event, const ci::mat4 &world_transform ) { return false; }
  virtual bool    mouseDown( ci::app::MouseEvent &event, const ci::mat4 &world_transform ) { return false; }
  virtual bool    mouseDrag( ci::app::MouseEvent &event, const ci::mat4 &world_transform ) { return false; }
  virtual bool    mouseUp( ci::app::MouseEvent &event, const ci::mat4 &world_transform ) { return false; }

  //! Returns true if \a point lies within our interaction_bounds
  bool            contains( const ci::vec2 &point, const ci::mat4 &world_transform );

  ci::vec2       bounds_padding;
  ci::Rectf       interaction_bounds;
};

/**
 Basic button interaction.
 If you want to style based on state, add a BehaviorComponent that reads state
 on update and changes your style component(s).
 */
struct ButtonComponent : public GuiComponent
{
  virtual bool    touchesBegan( ci::app::TouchEvent &event, const ci::mat4 &world_transform );
  virtual bool    touchesMoved( ci::app::TouchEvent &event, const ci::mat4 &world_transform );
  virtual bool    touchesEnded( ci::app::TouchEvent &event, const ci::mat4 &world_transform );
  virtual bool    mouseDown( ci::app::MouseEvent &event, const ci::mat4 &world_transform );
  virtual bool    mouseDrag( ci::app::MouseEvent &event, const ci::mat4 &world_transform );
  virtual bool    mouseUp( ci::app::MouseEvent &event, const ci::mat4 &world_transform );

  //! Returns true if a tracked touch is inside component's bounds.
  bool isHovering() const { return _is_hovering; }

  std::function<void ()>  select_fn;
private:
  bool      _is_hovering;
  uint32_t  _tracked_touch;
};

struct ScrollComponent : public GuiComponent
{

};

/**

 */
struct SliderComponent : public GuiComponent
{

};


/**
  Gui needs to know about size, transformation, and a gui action.
  This is definitely a special case relative to other component pieces.

  Maybe make this a gui renderer that handles sliders properly...
 */
class GuiSystem : public treent::System<GuiSystem>, public treent::Receiver<GuiSystem>
{
public:

  GuiSystem();
  ~GuiSystem();
private:
};

} // treent::

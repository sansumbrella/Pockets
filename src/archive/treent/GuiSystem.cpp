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

#include "GuiSystem.h"

using namespace cinder;

#if 1

namespace treent
{

//
//  MARK: - GuiComponent
//

bool GuiComponent::contains( const ci::vec2 &point, const mat4 &world_transform )
{
  return interaction_bounds.contains( ci::vec2( ci::inverse( world_transform ) * ci::vec4( point, 0.0f, 1.0f ) ) );
}

//
//  MARK: - ButtonComponent
//

bool ButtonComponent::touchesBegan( ci::app::TouchEvent &event, const mat4 &world_transform )
{
  for( auto &touch : event.getTouches() )
  {
    if( contains( touch.getPos(), world_transform ) )
    {
      _tracked_touch = touch.getId();
      _is_hovering = true;
      return true;
    }
  }
  return false;
}
bool ButtonComponent::touchesMoved( ci::app::TouchEvent &event, const mat4 &world_transform )
{
  for( auto &touch : event.getTouches() )
  {
    if( touch.getId() == _tracked_touch )
    {
      if( contains( touch.getPos(), world_transform ) )
        _is_hovering = true;
      else
        _is_hovering = false;
    }
  }
  // don't capture moving touches
  return false;
}
bool ButtonComponent::touchesEnded( ci::app::TouchEvent &event, const mat4 &world_transform )
{
  bool selected = false;
  for( auto &touch : event.getTouches() )
  {
    if( touch.getId() == _tracked_touch )
    {
      _tracked_touch = 0; // reset tracked touch
      _is_hovering = false;
      if( contains( touch.getPos(), world_transform ) )
      {
        selected = true;
        break;
      }
    }
  }

  if( selected && select_fn )
  { // call function last, as it may destroy this object
    select_fn();
  }
  return false;
}
bool ButtonComponent::mouseDown( ci::app::MouseEvent &event, const mat4 &world_transform )
{
  if( contains( event.getPos(), world_transform ) )
  {
    _tracked_touch = MOUSE_ID;
    _is_hovering = true;
    return true;
  }
  return false;
}
bool ButtonComponent::mouseDrag( ci::app::MouseEvent &event, const mat4 &world_transform )
{
  _is_hovering = contains( event.getPos(), world_transform );

  return false;
}
bool ButtonComponent::mouseUp( ci::app::MouseEvent &event, const mat4 &world_transform )
{
  bool selected = false;
  _tracked_touch = 0;
  if( contains( event.getPos(), world_transform ) )
    selected = true;

  _is_hovering = false;
  if( selected && select_fn )
  {
    select_fn();
  }

  return false;
}

}

#endif

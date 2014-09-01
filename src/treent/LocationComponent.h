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
#include "Treent.h"
#include "cinder/MatrixAffine2.h"
#include "cinder/Tween.h"

namespace treent
{
  typedef std::shared_ptr<struct LocationComponent> LocationComponentRef;
  /**
   A Component storing the basic positional information for an Entity
   Position, Rotation, and Scale
   Scales and rotates around the Registration Point when using toMatrix()

   Used by RenderSystem to transform RenderMesh component vertices
   Updated by movement systems (Physics, Custom Motion)
   No assumption is made about the units used
  */
  struct LocationComponent : Component<LocationComponent>
  {
    LocationComponent() = default;
    LocationComponent( const ci::vec2 &pos, const ci::vec2 &registration, float rot ):
    position( pos ),
    registration_point( registration ),
    rotation( rot )
    {}

    ci::Anim<ci::vec2>          position = ci::vec2( 0 );
    ci::Anim<ci::vec2>          registration_point = ci::vec2( 0 );
    ci::Anim<float>             rotation = 0.0f;
    ci::Anim<ci::vec2>          scale = ci::vec2( 1 );
    ci::mat4                    matrix;

    void updateMatrix( const ci::mat4 &parentMatrix );
    //! returns a matrix that will transform points based on LocationComponent properties
    ci::mat4  calcLocalMatrix() const;
  };

} // treent::

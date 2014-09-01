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

namespace pockets
{
  /**
   Locus2D:

   Stores Position, Rotation, and Scale
   Enables direct manipulation of positional aspects and composing transforms.
   Scales and rotates around the Registration Point when using toMatrix()
  */
  typedef std::shared_ptr<class Locus2D> Locus2DRef;
  struct Locus2D
  {
    Locus2D() = default;
    Locus2D( const ci::vec2 &pos, const ci::vec2 &registration, float rot, Locus2DRef parent=nullptr ):
    position( pos ),
    registration_point( registration ),
    rotation( rot ),
    parent( parent )
    {}
    ci::vec2           position = ci::vec2( 0 );
    ci::vec2           registration_point = ci::vec2( 0 );
    float               rotation = 0.0f;
    ci::vec2           scale = ci::vec2( 1.0f, 1.0f );
    //! returns total rotation including any accumulated from parents
    float               getRotation() const;
    //! returns total scale including any accumulated from parents
    ci::vec2           getScale() const;
    //! returns total position including any accumulated from parents
    ci::vec2           getPosition() const;
    //! transform parent; toMatrix() is multiplied by parent->toMatrix() if present
    Locus2DRef          parent = nullptr;
    //! returns a matrix that will transform points based on Locus properties
    ci::mat4  toMatrix() const;
    //! remove parent after composing its transform into our own
    void                detachFromParent();
  };


  /**
   Locus3D:

   Stub for a 3d equivalent to Locus2D
   */
  typedef std::shared_ptr<class Locus3D> Locus3DRef;
  struct Locus3D
  {
    // TODO
    ci::vec3         mPosition;
    ci::quat         mOrientation;
    float             mScale;
  };
}

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
#include "cinder/Vector.h"
#include "cinder/MatrixAffine2.h"
#include "cinder/Quaternion.h"

namespace pockets
{
  typedef std::shared_ptr<struct Locus2D> Locus2DRef;
  typedef std::shared_ptr<struct Locus3D> Locus3DRef;

  /**
   Locus2D:

   Stores Position, Rotation, and Scale
   Enables direct manipulation of positional aspects and composing transforms.
   Scales and rotates around the Registration Point when using toMatrix()
  */
  struct Locus2D
  {
    Locus2D() = default;
    Locus2D( const ci::Vec2f &pos, const ci::Vec2f &registration, float rot, Locus2DRef parent=nullptr ):
      position( pos ),
      registration_point( registration ),
      rotation( rot ),
      parent( parent )
    {}

    //! Local world offset.
    ci::Vec2f           position = ci::Vec2f::zero();
    //! Local origin for scale and rotation.
    ci::Vec2f           registration_point = ci::Vec2f::zero();
    //! Local rotation in radians.
    float               rotation = 0.0f;
    //! Local scale.
    ci::Vec2f           scale = ci::Vec2f::one();
    //! transform parent; toMatrix() is multiplied by parent->toMatrix() if present
    Locus2DRef          parent = nullptr;

    //! Returns total rotation including any accumulated from parents.
    float               worldRotation() const;
    //! Returns total scale including any accumulated from parents.
    ci::Vec2f           worldScale() const;
    //! Returns total position including any accumulated from parents.
    ci::Vec2f           worldPosition() const;
    //! Returns a matrix combining all transformations multiplied by parent's matrix.
    ci::MatrixAffine2f  toMatrix() const;

    //! Remove parent after composing its transformations into our own.
    void                detachFromParent();
  };


  /**
   Locus3D:

   Stub for a 3d equivalent to Locus2D
   */
  struct Locus3D
  { // TODO: provide nice conversion method to matrix, parenting
    ci::Vec3f         position      = ci::Vec3f::zero();
    ci::Vec3f         pivot         = ci::Vec3f::zero();
    ci::Quatf         orientation   = ci::Quatf::identity();
    ci::Vec3f         scale         = ci::Vec3f::one();
  };
}

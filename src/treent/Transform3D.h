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
#include "Treent.h"
#include "cinder/MatrixAffine2.h"
#include "cinder/Tween.h"

namespace treent
{
  typedef std::shared_ptr<struct Transform3D> Transform3DRef;
  /**
   A Component storing the basic positional information for an Entity
   Position, Rotation, and Scale
   Scales and rotates around the Registration Point when using toMatrix()

   Used by RenderSystem to transform RenderMesh component vertices
   Updated by movement systems (Physics, Custom Motion)
   No assumption is made about the units used
  */
  struct Transform3D : Component<Transform3D>
  {
    Transform3D() = default;
    Transform3D( const ci::Vec3f &pos, const ci::Vec3f &pivot ):
    position( pos ),
    pivot( pivot )
    {}

    ci::Vec3f       position  = ci::Vec3f::zero();
    ci::Vec3f       pivot     = ci::Vec3f::zero();
    ci::Quatf       orientation  = ci::Quatf::identity();
    ci::Vec3f       scale     = ci::Vec3f::one();
    ci::Matrix44f   matrix    = ci::Matrix44f::identity();

    void updateMatrix( ci::Matrix44f parentMatrix );
    //! returns a matrix that will transform points based on Transform3D properties
    ci::Matrix44f  calcLocalMatrix() const;
  };

} // treent::

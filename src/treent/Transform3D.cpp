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

#include "treent/Transform3D.h"

using namespace cinder;
using namespace treent;

Matrix44f Transform3D::calcLocalMatrix() const
{
  const auto axis = orientation.getAxis();
  const auto angle = orientation.getAngle();

  Matrix44f mat;
  mat.setTranslate( position + pivot );

  if( ci::math<float>::abs( angle ) > 0.0 ) {
    mat.rotate( axis, angle );
  }

  mat.scale( scale );
  mat.translate( - pivot );
  return mat;
}

void Transform3D::updateMatrix( ci::Matrix44f parentMatrix )
{
  const auto axis = orientation.getAxis();
  const auto angle = orientation.getAngle();

  parentMatrix.translate( position + pivot );

  if( ci::math<float>::abs( angle ) > 0.0 ) {
    parentMatrix.rotate( axis, angle );
  }

  parentMatrix.scale( scale );
  parentMatrix.translate( - pivot );
  matrix = parentMatrix;
}

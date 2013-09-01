/*
 * Copyright (c) 2013 David Wicks
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
#include "Types.h"

namespace pockets
{

  typedef std::shared_ptr<class Locus2d>  Locus2dRef;
  /**
   A nestable position and orientation in 2d space.
   Will inherit transformations from a parent Locus, parents know nothing about
   children. Inteded for use in an vector-like container for fast iteration, with
   explicit evaluation of the transformation matrix.
   */
  class Locus2d
  {
  public:
    ci::Vec2f           getLoc() const { return mLoc; }
    void                setLoc( const ci::Vec2f &loc ){ mLoc = loc; setDirty(); }
    //! get local rotation
    float               getRotation() const { return mRotation; }
    //! get final rotation after all parent transformations
    float               getAccumulatedRotation() const { return mParent ? mParent->getAccumulatedRotation() + mRotation : mRotation; }
    //! set rotation to \a radians
    void                setRotation( float radians ){ mRotation = radians; setDirty(); }
    //! increment rotation by \a radians
    void                rotate( float radians ){ mRotation += radians; setDirty(); }
    //! returns the point around which rotation occurs, in local coordinates
    ci::Vec2f           getRegistrationPoint() const { return mRegistrationPoint; }
    //! set the point around which rotation occurs, in local coordinates
    void                setRegistrationPoint( const ci::Vec2f &loc ){ mRegistrationPoint = loc; setDirty(); }
    //! returns transformation matrix multiplied by parent (if any)
    inline ci::MatrixAffine2f getTransform() { calculateTransform(); return mParent ? mParent->getTransform() * mTransform : mTransform; }
    inline ci::MatrixAffine2f getTransform() const { return mParent ? mParent->getTransform() * mTransform : mTransform; }
    //! returns this locus' local transformation matrix, with no parent transforms
    inline ci::MatrixAffine2f getLocalTransform() { calculateTransform(); return mTransform; }
    inline ci::MatrixAffine2f getLocalTransform() const { return mTransform; }
    //! set a locus as a parent for this one; we then inherit transformations
    void                setParent( Locus2dRef parent ){ mParent = parent; }
    //! stop having a parent locus
    void                unsetParent(){ mParent.reset(); }
    //! calculates the local transformation matrix if it has changed
    void                calculateTransform();
    //! we've made changes and will need updating
    void                setDirty(){ mDirty = true; }
    //! conversion to Matrix44f for OpenGL transformation; just pass the Locus to gl::multModelView();
    operator            ci::Matrix44f() const { return ci::Matrix44f(getTransform()); }

    Vertex2d            transform( const Vertex2d &vertex ) const;
  private:
    ci::MatrixAffine2f          mTransform;
    ci::Vec2f                   mLoc = ci::Vec2f::zero();
    ci::Vec2f                   mRegistrationPoint = ci::Vec2f::zero();
    float                       mRotation = 0.0f;
    Locus2dRef                  mParent = nullptr;
    bool                        mDirty = true;
  };
} // pockets::

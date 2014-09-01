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
#include "Locus.h"
#include <array>

/**
 Sprite data and a few representations for that data.

 SpriteData: holds information about where a graphic lives on a sheet
 Sprite: renderable sprite
 SpriteAnimation: renderable sequence of sprites
 */
namespace pockets
{
  /**
   SpriteData holds information about a sprite's dimensions and texture coordinates.
   */
  class SpriteData
  {
  public:
    SpriteData(){}; // need default constructor to use in a std::map
    //! construct sprite data from its pixel size and normalized texture coordinates
    SpriteData( const ci::ivec2 &pixel_size, const ci::Rectf &textureBounds, const ci::vec2 &registration_point=ci::vec2( 0 ) );
    //! returns the Sprite's pixel dimensions
    inline ci::ivec2 getSize() const { return mSize; }
    //! returns the normalized texture coordinates of the sprite graphic
    inline ci::Rectf getTextureBounds() const { return mTextureBounds; }
    //! returns the registration point of the sprite, treated as the origin of the artwork
    inline ci::vec2 getRegistrationPoint() const { return mRegistrationPoint; }
    void setRegistrationPoint( const ci::ivec2 &point ){ mRegistrationPoint = point; }
  private:
    // assign values where default-constructed sprite data is obvious
    ci::vec2 mRegistrationPoint = ci::vec2( 0 );
    ci::ivec2 mSize = ci::ivec2( 48, 48 );
    ci::Rectf mTextureBounds = ci::Rectf(0,0,1,1);
  };

  /**
   Sprite renders a textured rectangle based on SpriteData specifications.
   */
  typedef std::shared_ptr<class Sprite> SpriteRef;
  class Sprite
  {
  public:
    Sprite() = default;
    explicit Sprite( const SpriteData &data );
    ~Sprite();
    //! clip region with rectangle; only parts contained by rect will be visible
    void        clipBy( const ci::Rectf &rect );
    //! set portion of texture to render (used by clipBy)
    void        setRegion( const ci::Rectf &portion );
    //! render the sprite to screen with local transformations
    void        render();
    std::vector<Vertex2d> getVertices() { updateTransformedVertices(); return mTransformedVertices; }
    //! draw the sprite without applying transform/tint/etc
    void        draw();
    void        setTint( const ci::ColorA &color );
    ci::ColorA  getTint() const { return mTint; }
    void        setRegistrationPoint( const ci::ivec2 &point )
    { mData.setRegistrationPoint( point ); updatePositions( ci::Rectf( ci::vec2( 0 ), mData.getSize() ) ); }
    ci::ivec2   getSize() const { return mData.getSize(); }
    ci::ivec2   getLoc() const { return mLocus.getLoc(); }
    void        setLoc( const ci::ivec2 &loc ){ setDirty(); mLocus.setLoc( loc ); }
    ci::vec2   getRegistrationPoint() const { return mData.getRegistrationPoint(); }
    bool        contains( const ci::vec2 &point );
    //! returns boundaries in local coordinates
    ci::Rectf   getLocalBounds() const { return ci::Rectf( ci::vec2( 0 ), getSize() ) - getRegistrationPoint(); }
    void        setLoc( const ci::vec2 &loc ){ mLocus.setLoc( loc ); }
    //    void setParent( Locus2dRef parent ){ mLocus.setParent( parent ); }
    // assume it will change and set us dirty
    Locus2d&    getLocus(){ setDirty(); return mLocus; }
  private:
    ci::ColorA                mTint;
    SpriteData                mData;
    Locus2d                   mLocus;
    std::array<Vertex2d, 4>   mVertices;
    std::vector<Vertex2d>     mTransformedVertices;
    bool                      mVerticesDirty = true;
    void updatePositions( const ci::Rectf &position_rect );
    void updateTexCoords( const ci::Rectf &tex_coord_rect );
    void updateTransformedVertices();
    void setDirty(){ mVerticesDirty = true; }
  };
} // pockets::

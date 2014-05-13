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
#include "SimpleRenderer.h"
#include "Sprite.h"
#include <array>

namespace pockets
{
  /**
    A line connecting two points.
    Expands line geometry in two dimensions to allow for very fat
    strokes, antialiased edges, and custom-drawn end caps.
    Can
  */
  typedef std::shared_ptr<class ExpandedLine2d> ExpandedLine2dRef;
  typedef std::unique_ptr<class ExpandedLine2d> ExpandedLine2dUniqueRef;
  class ExpandedLine2d : public SimpleRenderer::Renderable
  {
  public:
    ExpandedLine2d( const ci::Vec2f &begin, const ci::Vec2f &end );
    ~ExpandedLine2d();
    //! reset the line end points
    void setEndPoints( const ci::Vec2f &begin, const ci::Vec2f &end );
    void setWidth( float width ){ mWidth = width; buildOutline( mBegin, mBegin + mRay ); }
    //! scales the length of the line with the beginning fixed (end point moves)
    void scaleLength( float scale );
    //! scales the length of the line with the end fixed (beginning moves)
    void scaleLengthInverse( float scale );
    //! Use sprite data to determine width of line and set texture coordinates
    void matchSprite( const SpriteData &sprite );
    //! returns the total length of the line (without scaling)
    float getLength() const { return mRay.length(); }
    void render();
    static ExpandedLine2dUniqueRef create( const ci::Vec2f &begin, const ci::Vec2f &end )
    {
      return ExpandedLine2dUniqueRef( new ExpandedLine2d( begin, end ) );
    }
  private:
    ci::Vec2f                 mBegin, mRay;
    std::array<ci::Vec2f, 8>  mPositions;
    std::array<ci::Vec2f, 8>  mTexCoords;
    float                     mWidth = 6.0f;
    float                     mScale = 1.0f;
    void  buildOutline( const ci::Vec2f &begin, const ci::Vec2f &end );
    void  buildTexCoords( const ci::Rectf &bounds = ci::Rectf( 0, 0, 1, 1 ) );
  };

}
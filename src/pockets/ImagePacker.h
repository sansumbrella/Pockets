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
#include "cinder/Json.h"
#include "cinder/Surface.h"
#include "cinder/Rect.h"
#include "cinder/Font.h"

namespace pockets
{

/**
 Simple image packing
 Places all elements, largest-first, into a single Surface
 Height expands to fit

 Though spritesheet generation is generally an offline task, it
 could be useful for things like super-8-style-recording and gif-playback
*/
class ImagePacker
{
  public:
  /*
   Source data for a sprite.
   Contains positional information and bitmap data.
   Converts to Json for export.
   */
  class ImageData
  {
  public:
    ImageData( ci::Surface surface, const std::string &id ):
    mSurface( surface )
    , mId( id )
    {}
    ci::Rectf           getPlacedBounds() const { return mSurface.getBounds() + getLoc(); }
    ci::Area            getBounds() const { return mSurface.getBounds(); }
    const ci::Surface&  getSurface() const { return mSurface; }
    ci::Vec2i           getLoc() const { return mLoc; }
    void                setLoc( const ci::Vec2i &loc ){ mLoc = loc; }
    void                setRegistrationPoint( const ci::Vec2i &reg ){ mRegistrationPoint = reg; }
    ci::Vec2i           getRegistrationPoint() const { return mRegistrationPoint; }
    ci::Vec2i           getSize() const { return mSurface.getSize(); }
    int                 getWidth() const { return mSurface.getWidth(); }
    int                 getHeight() const { return mSurface.getHeight(); }
    std::string         getId() const { return mId; }
    ci::JsonTree        toJson()
    {
      using ci::JsonTree;
      JsonTree tree;
      tree.pushBack( JsonTree( "id", mId ) );
      tree.pushBack( JsonTree( "x1", mLoc.x ) );
      tree.pushBack( JsonTree( "y1", mLoc.y ) );
      tree.pushBack( JsonTree( "x2", mLoc.x + getBounds().getWidth() ) );
      tree.pushBack( JsonTree( "y2", mLoc.y + getBounds().getHeight() ) );
      tree.pushBack( JsonTree( "rx", mRegistrationPoint.x ) );
      tree.pushBack( JsonTree( "ry", mRegistrationPoint.y ) );
      return tree;
    }
  private:
    ci::Surface     mSurface;
    ci::Vec2i       mLoc = ci::Vec2i::zero();
    ci::Vec2i       mRegistrationPoint = ci::Vec2i::zero();
    std::string     mId;
  };
  typedef std::shared_ptr<ImageData> ImageDataRef;
	ImagePacker();
	~ImagePacker();

  //! add an image to the sheet. If trim_alpha, trims image bounds to non-alpha area
  ImageDataRef              addImage( const std::string &id, ci::Surface surface, bool trim_alpha=false );

  //! add the specified glyphs from a font; id is equal to the character, e.g. "a"
  std::vector<ImageDataRef> addGlyphs( const ci::Font &font, const std::string &glyphs, const std::string &id_prefix="", bool trim_alpha=false );

  //! add the specified string set in a font
  ImageDataRef              addString( const std::string &id, const ci::Font &font, const std::string &str, bool trim_alpha=false );

  //! assign positions to images
  void                      calculatePositions( const ci::Vec2i &padding, const int width=1024 );

  //! use a scanline algorithm to calculate positions (slower to run, more compact)
  void                      calculatePositionsScanline( const ci::Vec2i &padding, const int width=1024 );

  //! generates a surface containing all added images in their packed locations
  //! premultiply unless you are using this spritesheet on iOS, in which case XCode will do that later
  ci::Surface               packedSurface( bool premultiply=false );

  //! returns a JSON-formatted description of all images and their packed locations
  ci::JsonTree              surfaceDescription();

  //! remove all images from packer
  void                      clear() { mImages.clear(); }

  // Collection traversal.
  std::vector<ImageDataRef>::iterator begin(){ return mImages.begin(); }
  std::vector<ImageDataRef>::iterator end(){ return mImages.end(); }
private:
  //! width should be set to maximum desired width
  uint32_t                  mWidth = 1024;
  //! height expands as elements are added
  uint32_t                  mHeight = 1;
  std::vector<ImageDataRef> mImages;
};
} // ns pockets
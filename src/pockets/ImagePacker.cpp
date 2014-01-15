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

#include "ImagePacker.h"
#include "cinder/Text.h"
#include "cinder/ip/Trim.h"
#include "cinder/ip/Fill.h"
#include "cinder/ip/Premultiply.h"

using namespace ci;
using namespace std;
using namespace pockets;

ImagePacker::~ImagePacker()
{}

ImagePacker::ImageDataRef ImagePacker::addImage( const std::string &id, ci::Surface surface, bool trim_alpha )
{
  if( trim_alpha )
  {
    Area bounds = ip::findNonTransparentArea( surface, surface.getBounds() );
    assert( bounds.getWidth() <= surface.getWidth() );
    assert( bounds.getHeight() <= surface.getHeight() );
    Surface trimmed_copy( bounds.getWidth(), bounds.getHeight(), true, SurfaceChannelOrder::RGBA );
    trimmed_copy.copyFrom( surface, bounds, -bounds.getUL() );
    surface = trimmed_copy;
  }
  mImages.push_back( make_shared<ImageData>( surface, id ) );
  return mImages.back();
}

vector<ImagePacker::ImageDataRef> ImagePacker::addGlyphs( const ci::Font &font, const string &glyphs, const string &id_prefix, bool trim_alpha )
{
  vector<ImageDataRef> ret;
  for( const char glyph : glyphs )
  {
    ret.push_back( addString( id_prefix + toString<char>(glyph), font, toString<char>(glyph), trim_alpha ));
  }
  return ret;
}

ImagePacker::ImageDataRef ImagePacker::addString( const string &id, const Font &font, const string &str, bool trim_alpha )
{
  TextLayout layout;
  layout.clear( ColorA( 0, 0, 0, 0 ) );
  layout.setFont( font );
  layout.setColor( ColorA::white() );
  layout.addLine( str );
  Surface image = layout.render( true, false );
  return addImage( id, image, trim_alpha );
}

JsonTree ImagePacker::surfaceDescription()
{
  JsonTree description;
  JsonTree metaData = JsonTree::makeObject("meta");
  metaData.pushBack( JsonTree("created_by", "David Wicks' pockets::ImagePacker") );
  metaData.pushBack( JsonTree("width", mWidth ) );
  metaData.pushBack( JsonTree("height", mHeight ) );
  description.pushBack( metaData );

  JsonTree sprites = JsonTree::makeArray("sprites");
  for( ImageDataRef sprite : mImages )
  {
    sprites.pushBack( sprite->toJson() );
  }
  description.pushBack( sprites );

  return description;
}

Surface ImagePacker::packedSurface( bool premultiply )
{
  Surface output( mWidth, mHeight, true, SurfaceChannelOrder::RGBA );
  ip::fill( &output, ColorA( 0, 0, 0, 0 ) );
  for( ImageDataRef sprite : mImages )
  {
    output.copyFrom( sprite->getSurface(), sprite->getBounds(), sprite->getLoc() );
  }
  // XCode premultiplies anything going to an iOS device (ignores flag telling it not to)
  // Double-premultiplication makes everything have dark edges
  // So we'll just premultiply on the client side if the graphics aren't already
  if( premultiply )
  {
    ip::premultiply( &output );
  }
  return output;
}

void ImagePacker::calculatePositions( const ci::Vec2i &padding, const int width )
{
  //  auto rev_area_compare = []( const ImageData &lhs, const ImageData &rhs )
  //  {
  //    return lhs.getBounds().calcArea() > rhs.getBounds().calcArea();
  //  };
  auto rev_height_compare = []( const ImageDataRef &lhs, const ImageDataRef &rhs )
  {
    return lhs->getBounds().getHeight() > rhs->getBounds().getHeight();
  };
  sort( mImages.begin(), mImages.end(), rev_height_compare );
  Vec2i loc( 0, 0 );
  int bottom_y = 0;
  for( ImageDataRef sprite : mImages )
  {
    if( loc.x + sprite->getBounds().getWidth() > width )
    {
      loc.y = bottom_y + padding.y;
      loc.x = 0;
    }
    sprite->setLoc( loc );
    loc.x += sprite->getBounds().getWidth() + padding.x;
    bottom_y = math<int>::max( sprite->getBounds().getHeight() + loc.y, bottom_y );
  }
  mWidth = width;
  mHeight = math<int>::max( bottom_y, width );
}

void ImagePacker::calculatePositionsScanline( const Vec2i &padding, const int width )
{
  // auto rev_area_compare = []( const ImageDataRef &lhs, const ImageDataRef &rhs )
  // {
  //   return lhs->getBounds().calcArea() > rhs->getBounds().calcArea();
  // }
  auto rev_height_compare = []( const ImageDataRef &lhs, const ImageDataRef &rhs )
  {
    return lhs->getBounds().getHeight() > rhs->getBounds().getHeight();
  };
  sort( mImages.begin(), mImages.end(), rev_height_compare );
  int bottom_y = 0;
  // place largest image at top-left
  mImages.at( 0 )->setLoc( padding );
  for( int i = 1; i < mImages.size(); ++i )
  { // for each following image, start at top-left and look on each
    // pixel row for a potential free space
    Vec2i loc( padding );
    auto img = mImages.at( i );
    bool placed = false;
    while ( !placed )
    {
      for( int j = 0; j < i; ++j )
      { // check whether we're inside the bounds of any placed image (including padding)
        auto bounds = mImages.at( j )->getPlacedBounds().inflated( padding );
        if( bounds.contains( loc ) )
        { // jump to right edge of image
          loc.x = bounds.getX2();
        }
      }
      
      if( loc.x + img->getWidth() < width - padding.x )
      { // we fit on the page, though we may still overlap other images
        // tentatively set the image location here
        img->setLoc( loc );
        placed = true;
        // shrink potential bounds by one, since sharing a vertex counts as Rectf intersecting
        auto potential_bounds = img->getPlacedBounds().inflated( Vec2i( -1, -1 ) );
        for( int j = 0; j < i; ++j )
        { // check that the placed boundaries don't overlap any other placed image
          auto bounds = mImages.at( j )->getPlacedBounds().inflated( padding );
          if( potential_bounds.intersects( bounds ) )
          { // a collision was found, so we'll continue the loop
            placed = false;
          }
        }
        bottom_y = math<int>::max( img->getBounds().getHeight() + loc.y, bottom_y );
      }
      // move to next row of pixels for continued evaluation
      loc.x = padding.x;
      loc.y += 1;
    }
  }
  mWidth = width;
  mHeight = math<int>::max( bottom_y, width );
}


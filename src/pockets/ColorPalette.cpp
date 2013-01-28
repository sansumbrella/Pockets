//
//  ColorPalette.cpp
//  Energy
//
//  Created by David Wicks on 1/28/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "ColorPalette.h"
#include "cinder/CinderMath.h"

using namespace pockets;
using namespace cinder;

ColorPalette::ColorPalette()
{}

ColorPalette::~ColorPalette()
{}

ColorA ColorPalette::getColor(float t)
{
  return ColorA( 1.0f, 0.0f, 0.0f, 0.0f );
}


ColorA ColorPalette::getColorClamped(float t)
{
	return getColor( math<float>::clamp( t, 0.0f, 1.0f ) );
}

ColorA ColorPalette::getColorWrapped(float t)
{
  if( t > 1.0f ){ t = t - math<float>::floor( t ); }
  else if( t < 0.0f ){ t = 1.0f + (t - math<float>::ceil( t )); }
  return getColor( t );
}

SurfacePalette::SurfacePalette( const Surface &surf ):
mSurface( surf )
{}

SurfacePaletteRef SurfacePalette::create(const ci::Surface &surf)
{
  return SurfacePaletteRef( new SurfacePalette( surf ) );
}

ColorA SurfacePalette::getColor( float t )
{
  return mSurface.getPixel( Vec2i( math<float>::floor( t * (mSurface.getWidth() - 1) + 0.5f ), 0 ) );
}

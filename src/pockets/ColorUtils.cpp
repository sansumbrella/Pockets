/*
 * Copyright (c) 2013, David Wicks
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

#include "ColorUtils.h"
#include "cinder/CinderMath.h"

using namespace pockets;
using namespace cinder;

ColorPalette::~ColorPalette()
{}

ColorA ColorPalette::getColor(float t) const
{
  return ColorA( 1.0f, 0.0f, 0.0f, 0.0f );
}


ColorA ColorPalette::getColorClamped(float t) const
{
	return getColor( math<float>::clamp( t, 0.0f, 1.0f ) );
}

ColorA ColorPalette::getColorWrapped(float t) const
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
  SurfacePaletteRef ret( new SurfacePalette( surf ) );
  return ret;
}

ColorA SurfacePalette::getColor( float t ) const
{
  return mSurface.getPixel( Vec2i( math<float>::floor( t * (mSurface.getWidth() - 1) + 0.5f ), 0 ) );
}

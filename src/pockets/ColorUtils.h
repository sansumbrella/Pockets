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

#pragma once

#include "cinder/Color.h"

namespace pockets
{
  typedef std::shared_ptr<class ColorPalette> ColorPaletteRef;
  class ColorPalette
  {
  public:
    virtual ~ColorPalette();
    //! get the color at position t
    virtual ci::ColorA getColor( float t ) const;
    //! get color at position t clamped to [0, 1]
    ci::ColorA getColorClamped( float t ) const;
    //! get color at position t wrapped to [0, 1]
    ci::ColorA getColorWrapped( float t ) const;
  };

  typedef std::shared_ptr<class SurfacePalette> SurfacePaletteRef;
  class SurfacePalette : public ColorPalette
  {
  public:
    SurfacePalette( const ci::Surface &surf );
    void setSurface( const ci::Surface &surf ){ mSurface = surf; }
    ci::ColorA getColor( float t ) const;
    static SurfacePaletteRef create( const ci::Surface &surf );
  private:
    ci::Surface mSurface;
  };
}


//
//  ColorPalette.h
//  Energy
//
//  Created by David Wicks on 1/28/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

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


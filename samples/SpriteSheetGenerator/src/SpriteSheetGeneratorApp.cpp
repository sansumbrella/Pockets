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

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/ip/Blend.h"
#include "cinder/ip/Resize.h"
#include "cinder/Json.h"
#include "cinder/params/Params.h"

#include "ImagePacker.h"

using namespace ci;
using namespace ci::app;
using namespace std;

/**
Generates a single-texture spritesheet and json descriptor file.

Drag-and-drop to add images to the spritesheet.
Ids are derived from filenames without extension.

Usage:
1. Run app
2. Drag/drop image files into app
3. Hit save
4. Enjoy spritesheet (maybe with SpriteSheetTester)
*/
class SpriteSheetGeneratorApp : public AppNative {
  public:
  void prepareSettings( Settings *settings );
	void setup();
	void mouseDown( MouseEvent event );
  void keyPressed();
	void update();
	void draw();
  void fileDrop( FileDropEvent event );
  void saveSpriteSheet();
private:
  params::InterfaceGl mParams;
  const int           cOutputSize = 1024;
  float               mWindowScaling = 1.0f;
  int                 mHeight = 1;
  std::string         mFilename = "sprite_sheet";
  pk::ImagePacker     mImagePacker;
};

void SpriteSheetGeneratorApp::prepareSettings(Settings *settings)
{
  settings->setWindowSize( 1024, 768 );
}

void SpriteSheetGeneratorApp::setup()
{
  mParams = params::InterfaceGl( "SpriteSheet Generator", Vec2i( 200, 200 ) );
  mParams.addParam( "Preview scaling", &mWindowScaling, "min=0.1 max=2.0 step=0.05");
  mParams.addParam( "Output name", &mFilename );
  mParams.addButton( "Save sheet", [this](){ saveSpriteSheet(); } );

  Font font( "Hoefler Text", 48 );
  mImagePacker.addFont( font, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
  Font large_font( "Hoefler Text", 48 * 2 );
  mImagePacker.addFont( large_font, "*", true );
  mImagePacker.calculatePositions();
}

void SpriteSheetGeneratorApp::mouseDown( MouseEvent event )
{
}

void SpriteSheetGeneratorApp::fileDrop(cinder::app::FileDropEvent event)
{
  for( fs::path file : event.getFiles() )
  {
    if( fs::exists( file ) && fs::is_regular_file( file ) )
    {
      Surface img = loadImage( file );
      string id = file.stem().string();
      mImagePacker.addImage( img, file.stem().string(), file );
    }
  }
  mImagePacker.calculatePositions();
}

void SpriteSheetGeneratorApp::update()
{
}

void SpriteSheetGeneratorApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
  gl::pushModelView();
  gl::scale( mWindowScaling, mWindowScaling );
  gl::color( Color::white() );
  gl::enableAlphaBlending();
  mImagePacker.draw();
//  gl::color( Color( 1, 0, 1 ) );
//  gl::drawStrokedRect( Rectf( 0, 0, cOutputSize, mHeight ) );
  gl::popModelView();
  params::InterfaceGl::draw();
}

void SpriteSheetGeneratorApp::saveSpriteSheet()
{
  fs::path output_path = getAppPath().parent_path();
  if( output_path.stem() == "Debug" || output_path.stem() == "Release" )
  { // running from inside XCode or similar, so back up a bunch of directories
    output_path = output_path / "../../../output";
  }
  auto file = writeFile( output_path / (mFilename + ".json") );
  if( file )
  {
    mImagePacker.surfaceDescription().write( file );
    writeImage( output_path / (mFilename + ".png"), mImagePacker.packedSurface() );
  }
}

CINDER_APP_NATIVE( SpriteSheetGeneratorApp, RendererGl )

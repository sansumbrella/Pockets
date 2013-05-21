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
#include <set>

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
  void addFile( const fs::path &file );
  void saveSpriteSheet();
private:
  params::InterfaceGl mParams;
  const int           cOutputSize = 1024;
  float               mWindowScaling = 1.0f;
  Vec2f               mPreviewOffset;
  int                 mHeight = 1;
  std::string         mFilename = "sprite_sheet";
  pk::ImagePacker     mImagePacker;
  int                 mWidestImage = 0;
  ci::Vec2i           mMargin = { 20, 20 };
};

void SpriteSheetGeneratorApp::prepareSettings(Settings *settings)
{
  settings->setWindowSize( 1024, 768 );
}

void SpriteSheetGeneratorApp::setup()
{
  mParams = params::InterfaceGl( "SpriteSheet Generator", Vec2i( 200, 200 ) );
  mParams.setOptions( "", "position='50 450'" );
  mParams.addParam( "Preview scaling", &mWindowScaling, "min=0.1 max=2.0 step=0.05");
  mParams.addParam( "Preview offset", &mPreviewOffset.y );
  mParams.addParam( "Output name", &mFilename );
  mParams.addButton( "Save sheet", [this](){ saveSpriteSheet(); } );
}

void SpriteSheetGeneratorApp::mouseDown( MouseEvent event )
{
}

void SpriteSheetGeneratorApp::fileDrop(cinder::app::FileDropEvent event)
{
  for( fs::path file : event.getFiles() )
  {
    addFile( file );
  }
  mImagePacker.setWidth( mWidestImage * 3 + mMargin.x * 2 );
  mImagePacker.calculatePositions( mMargin );
}

void SpriteSheetGeneratorApp::addFile(const fs::path &file)
{
  set<string> extensions = { ".png", ".jpg", ".gif", ".tiff", ".tif", ".tga" };
  if( fs::exists( file ) )
  {
    cout << "Checking file: " << file << ", extension: " << file.extension().string() << endl;
    if( fs::is_regular_file( file ) && extensions.count(file.extension().string()) )
    {
      cout << "Adding file: " << file << endl;
      Surface img = loadImage( file );
      string id = file.stem().string();
      mWidestImage = max( img.getWidth(), mWidestImage );
      auto sprite = mImagePacker.addImage( id, img );
      sprite->setRegistrationPoint( Vec2i( sprite->getWidth() / 2, sprite->getHeight() ) );
    }
    else if( fs::is_directory( file ) )
    {
      for( auto iter = fs::directory_iterator( file ); iter != fs::directory_iterator(); ++iter )
      {
        addFile( *iter );
      }
    }
  }
}

void SpriteSheetGeneratorApp::update()
{
}

void SpriteSheetGeneratorApp::draw()
{
  mParams.draw();
}

void SpriteSheetGeneratorApp::saveSpriteSheet()
{
  fs::path output_path = getFolderPath();
  auto file = writeFile( output_path / (mFilename + ".json") );
  if( file )
  {
    mImagePacker.surfaceDescription().write( file );
    writeImage( output_path / (mFilename + ".png"), mImagePacker.packedSurface() );
  }
}

CINDER_APP_NATIVE( SpriteSheetGeneratorApp, RendererGl )

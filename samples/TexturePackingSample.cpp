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

#include "TexturePackingSample.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void TexturePackingSample::setup()
{
  mParams = params::InterfaceGl::create( "SpriteSheet Generator", Vec2i( 200, 200 ) );
  mParams->setOptions( "", "position='10 20'" );
  mParams->addButton( "Reset packer", [this](){ mImagePacker.clear(); } );
}

void TexturePackingSample::connect( ci::app::WindowRef window )
{
  storeConnection( window->getSignalFileDrop().connect( [this]( const FileDropEvent &event )
                                                       {
                                                         fileDrop( event );
                                                       } ) );
}

void TexturePackingSample::fileDrop(cinder::app::FileDropEvent event)
{
  for( fs::path file : event.getFiles() )
  {
    addFile( file );
  }
//  const int w = mWidestImage * 3 + mMargin.x * 2;
  mImagePacker.calculatePositions( { 1, 1 }, 1024 );
  saveSpriteSheet( "spritesheet-basic-pack" );
  mImagePacker.calculatePositionsScanline( { 1, 1 }, 1024 );
  saveSpriteSheet( "spritesheet-scanline-pack" );
}

void TexturePackingSample::addFile(const fs::path &file)
{
  set<string> extensions = { ".png", ".jpg", ".gif", ".tiff", ".tif", ".tga" };
  if( fs::exists( file ) )
  {
    if( fs::is_regular_file( file ) && extensions.count(file.extension().string()) )
    {
      Surface img = loadImage( file );
      string id = file.stem().string();
      cout << "File: " << file << endl;
      cout << "=> Id: " << id << endl;
      mWidestImage = max( img.getWidth(), mWidestImage );
      auto sprite = mImagePacker.addImage( id, img );
      sprite->setRegistrationPoint( sprite->getSize() / 2 );
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

void TexturePackingSample::draw()
{
  gl::clear( Color::white() );
  mParams->draw();
}

void TexturePackingSample::saveSpriteSheet( const string &filename )
{
  fs::path output_path = getDocumentsDirectory() / "pockets-output";
  auto file = writeFile( output_path / (filename + ".json") );
  if( file )
  {
    mImagePacker.surfaceDescription().write( file );
    writeImage( output_path / (filename + ".png"), mImagePacker.packedSurface() );
  }
}

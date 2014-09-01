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

#include "pockets/Scene.h"

#include "cinder/app/AppNative.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/ip/Blend.h"
#include "cinder/ip/Resize.h"
#include "cinder/Json.h"
#include "cinder/params/Params.h"

#include "ImagePacker.h"
#include <set>

/**
Generates a single-texture spritesheet and json descriptor file.

Ids are derived from filenames without extension.

Usage:
1. Drop image files into running app
2. Pick a directory for sheets to be saved in

*/
class TexturePackingSample : public pk::Scene
{
  public:
	void setup() override;
  void connect( ci::app::WindowRef window ) override;
	void draw() override;
  void fileDrop( ci::app::FileDropEvent event );
  void addFile( const ci::fs::path &file );
  void saveSpriteSheet( const std::string &filename );
private:
  ci::params::InterfaceGlRef mParams;
  const int                  cOutputSize = 1024;
  float                   mWindowScaling = 1.0f;
  ci::vec2               mPreviewOffset;
  int                     mHeight = 1;
  pk::ImagePacker         mImagePacker;
  int                     mWidestImage = 0;
  ci::ivec2               mMargin = { 20, 20 };
};

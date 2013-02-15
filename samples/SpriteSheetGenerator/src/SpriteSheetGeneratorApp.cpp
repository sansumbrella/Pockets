#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/ip/Blend.h"
#include "cinder/ip/Resize.h"
#include "cinder/Json.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

/*
 Source data for a sprite.
 Contains positional information and bitmap data.
 Drawable for previewing the texture.
 Converts to Json for export.

 Currently stores the original file since we might want to refer back to it when
 rebuilding sprite sheets. No support yet for reloading an existing sprite sheet
 with new versions of images.
 
 Usage:
 1. Run app
 2. Drag/drop image files into app
 3. Hit save
 4. Enjoy spritesheet (maybe with SpriteSheetTester)
*/
class Sprite
{
public:
  Sprite( Surface surface, const string &id, const fs::path &file_path ):
  mSurface( surface )
  , mTexture( surface )
  , mId( id )
  , mFilePath( file_path )
  {}
  Area            getBounds() const { return mSurface.getBounds(); }
  const Surface&  getSurface() const { return mSurface; }
  Vec2i           getLoc() const { return mLoc; }
  void            setLoc( const Vec2i &loc ){ mLoc = loc; }
  void            draw()
  {
    gl::draw( mTexture, mLoc );
  }
  JsonTree        toJson()
  {
    JsonTree tree;
    tree.pushBack( JsonTree( "id", mId ) );
    tree.pushBack( JsonTree( "x1", mLoc.x ) );
    tree.pushBack( JsonTree( "y1", mLoc.y ) );
    tree.pushBack( JsonTree( "x2", mLoc.x + getBounds().getWidth() ) );
    tree.pushBack( JsonTree( "y2", mLoc.y + getBounds().getHeight() ) );
    tree.pushBack( JsonTree( "file", mFilePath.string() ) );
    return tree;
  }
private:
  Surface     mSurface;
  gl::Texture mTexture;
  Vec2i       mLoc = Vec2i::zero();
  std::string mId;
  fs::path    mFilePath;
};

/**
Generates a single-texture spritesheet and json descriptor file.

Drag-and-drop to add images to the spritesheet.
Ids are derived from filenames without extension.
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
  void layoutSprites();
private:
  params::InterfaceGl mParams;
  vector<Sprite>      mSprites;
  const int           cOutputSize = 1024;
  float               mWindowScaling = 1.0f;
  Sprite              *mDraggedSprite = 0;
  int                 mHeight = 1;
  std::string         mFilename = "sprite_sheet";
};

void SpriteSheetGeneratorApp::prepareSettings(Settings *settings)
{
  settings->setWindowSize( 1024, 768 );
}

void SpriteSheetGeneratorApp::setup()
{
  mParams = params::InterfaceGl( "SpriteSheet Generator", Vec2i( 200, 200 ) );
  mParams.addParam( "Preview scaling", &mWindowScaling, "min=0.1 max=2.0 step=0.05");
//  mParams.addButton( "autolayout", [this](){ layoutSprites(); } );
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
    if( fs::exists( file ) && fs::is_regular_file( file ) )
    {
      Surface img = loadImage( file );
      string id = file.stem().string();
      mSprites.push_back( Sprite( img, id, file ) );
    }
  }
  layoutSprites();
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
  for( Sprite &sprite : mSprites )
  {
    sprite.draw();
  }
  gl::color( Color( 1, 0, 1 ) );
  gl::drawStrokedRect( Rectf( 0, 0, cOutputSize, mHeight ) );
  gl::popModelView();
  params::InterfaceGl::draw();
}

void SpriteSheetGeneratorApp::layoutSprites()
{
  auto rev_area_compare = []( const Sprite &lhs, const Sprite &rhs )
  {
    return lhs.getBounds().calcArea() > rhs.getBounds().calcArea();
  };
  sort( mSprites.begin(), mSprites.end(), rev_area_compare );
  Vec2i loc( 0, 0 );
  Vec2i padding( 1, 1 );
  int bottom_y = 0;
  for( Sprite &sprite : mSprites )
  {
    if( loc.x + sprite.getBounds().getWidth() > cOutputSize )
    {
      loc.y = bottom_y + padding.y;
      loc.x = 0;
    }
    sprite.setLoc( loc );
    loc.x += sprite.getBounds().getWidth() + padding.x;
    bottom_y = math<int>::max( sprite.getBounds().getHeight() + loc.y, bottom_y );
  }
  mHeight = bottom_y;
}

void SpriteSheetGeneratorApp::saveSpriteSheet()
{
  Surface output( cOutputSize, mHeight, true, SurfaceChannelOrder::RGBA );
  JsonTree json;
  JsonTree sprites = JsonTree::makeArray("sprites");
  JsonTree metaData = JsonTree::makeObject("meta");
  metaData.pushBack( JsonTree("created_by", "David Wicks' SpriteSheetGenerator") );
  metaData.pushBack( JsonTree("width", cOutputSize) );
  metaData.pushBack( JsonTree("height", mHeight ) );
  for( Sprite &sprite : mSprites )
  {
    output.copyFrom( sprite.getSurface(), sprite.getBounds(), sprite.getLoc() );
    sprites.pushBack( sprite.toJson() );
  }
  fs::path output_path = getAppPath().parent_path();
  if( output_path.stem() == "Debug" || output_path.stem() == "Release" )
  { // running from inside XCode or similar, so back up a bunch of directories
    output_path = output_path / "../../../output";
  }
  writeImage( output_path / (mFilename + ".png"), output );
  auto file = writeFile( output_path / (mFilename + ".json") );
  json.pushBack( metaData );
  json.pushBack( sprites );
  json.write( file );
}

CINDER_APP_NATIVE( SpriteSheetGeneratorApp, RendererGl )

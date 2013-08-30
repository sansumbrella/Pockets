#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Json.h"

#include "SpriteSheet.h"
#include "SpriteAnimation.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace pockets;

class SpriteAnimationApp : public AppNative {
  public:
  void prepareSettings( Settings *settings );
	void setup();
	void update();
	void draw();
private:
  SpriteSheetRef    mSpriteSheet;
  SpriteAnimation   mSpriteAnimation;
  SpriteAnimation   mJellyAnimation;
  double            mLastUpdate = 0.0;
};

void SpriteAnimationApp::prepareSettings( Settings *settings )
{
  settings->setWindowSize( 1024, 768 );
}

void SpriteAnimationApp::setup()
{
  mSpriteSheet = SpriteSheet::load( getAssetPath("spritesheet-scanline-pack.json") );

  auto sprites = mSpriteSheet->getSpriteNames();

  for( const string &name : sprites )
  { // hold for equivalent of 2 frames at 24fps
    mSpriteAnimation.addFrame( mSpriteSheet->getSpriteData(name), 1 );
    cout << "Sprite: " << name << endl;
  }
  mSpriteAnimation.setFrameRate( 12 );

  mSpriteAnimation.getLocus().setLoc( { getWindowWidth() / 2, getWindowHeight() - 100.0f } );
  mJellyAnimation.getLocus().setLoc( { getWindowWidth() * 0.25f, 300.0f } );
  try
  { // in case the original json is not properly formatted
    auto json = JsonTree( loadAsset( "animations.json" ) );
    mJellyAnimation.loadAnimationJson( json["jellyfish"], mSpriteSheet );
  }
  catch( std::exception &exc )
  {
    cout << "Exception: " << exc.what() << endl;
  }
}

void SpriteAnimationApp::update()
{
  float now = getElapsedSeconds();
  float deltaTime = now - mLastUpdate;
  mLastUpdate = now;
  mSpriteAnimation.step( deltaTime );
  mJellyAnimation.step( deltaTime );
}

void SpriteAnimationApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
  gl::color( Color::white() );
  mSpriteSheet->enableAndBind();
  mSpriteAnimation.render();
  mJellyAnimation.render();
}

CINDER_APP_NATIVE( SpriteAnimationApp, RendererGl )

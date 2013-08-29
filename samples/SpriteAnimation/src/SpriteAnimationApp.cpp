#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "SpriteSheet.h"
#include "SpriteAnimation.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace pockets;

class SpriteAnimationApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
private:
  SpriteSheetRef    mSpriteSheet;
  SpriteAnimation   mSpriteAnimation;
  double            mLastUpdate = 0.0;
};

void SpriteAnimationApp::setup()
{
  mSpriteSheet = SpriteSheet::load( getAssetPath("spritesheet-scanline-pack.json") );

  auto sprites = mSpriteSheet->getSpriteNames();

  for( const string &name : sprites )
  { // hold for equivalent of 2 frames at 24fps
    mSpriteAnimation.addFrame( mSpriteSheet->getSpriteData(name), 1 );
    cout << "Sprite: " << name << endl;
  }
  mSpriteAnimation.setFrameRate( 24 );

  mSpriteAnimation.getLocus().setLoc( { getWindowWidth() / 2, getWindowHeight() - 100.0f } );
}

void SpriteAnimationApp::mouseDown( MouseEvent event )
{
}

void SpriteAnimationApp::update()
{
  float now = getElapsedSeconds();
  float deltaTime = now - mLastUpdate;
  mLastUpdate = now;
  mSpriteAnimation.step( deltaTime );
}

void SpriteAnimationApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
  gl::color( Color::white() );
  mSpriteSheet->enableAndBind();
  mSpriteAnimation.render();
}

CINDER_APP_NATIVE( SpriteAnimationApp, RendererGl )

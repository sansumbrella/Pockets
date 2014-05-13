#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Json.h"

#include "SpriteSheet.h"
#include "SpriteAnimation.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace pockets;

/**

 Using SpriteAnimation to playback custom animations
 Loads animation data from file
 Creates a new animation at runtime
 File-loaded animations playback at animation-defined speed
 Mouse position determines speed and direction of total animation playback

 Loads a spritesheet and json descriptor.
 If no animation file is provided, assumes all sequential names (name-##)
 in spritesheet the basis of animations.
 Allows you to adjust hold times and ordering of frames.
 Exports animations in JSON format.

*/

class SpriteAnimationApp : public AppNative {
  public:
  void prepareSettings( Settings *settings );
	void setup();
	void update();
	void draw();
  void mouseMove( MouseEvent event ) override;
private:
  SpriteSheetRef    mSpriteSheet;
  SpriteAnimation               mTotalAnimation;
  std::vector<SpriteAnimation>  mSpriteAnimations;
  double            mLastUpdate = 0.0;
  float             mNormalizedMouseX = 1.0f;
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
    mTotalAnimation.addFrame( mSpriteSheet->getSpriteData(name), 1 );
  }
  mTotalAnimation.setFrameRate( 12 );
  mTotalAnimation.getLocus().setLoc( { getWindowWidth() / 2, getWindowHeight() - 100.0f } );

  try
  { // in case the original json is not properly formatted
    auto json = JsonTree( loadAsset( "animations.json" ) );
    float x = 10.0f;
    for( auto &child : json.getChildren() )
    {
      cout << "Animation: " << child.getKey() << endl;

      SpriteAnimation anim{};
      anim.loadAnimationJson( child, mSpriteSheet );
      x += anim.getSize().x / 2.0f;
      anim.getLocus().setLoc( { x, 300.0f } );
      x += anim.getSize().x / 2.0f;

      if( child.getKey() == "wormer" )
      {
        anim.getLocus().setRotation( M_PI / 2 );
      }
      mSpriteAnimations.push_back( anim );
    }
  }
  catch( std::exception &exc )
  {
    cout << "Exception: " << exc.what() << endl;
  }
}

void SpriteAnimationApp::mouseMove( MouseEvent event )
{
  mNormalizedMouseX = lmap<float>( event.getPos().x, 0, getWindowWidth(), 0.0f, 1.0f );
}

void SpriteAnimationApp::update()
{
  float now = getElapsedSeconds();
  float deltaTime = now - mLastUpdate;
  mLastUpdate = now;

  // get a time step based on mouse position (yep, negative time works, too)
  float total_step = lerp( -deltaTime * 2.0f, deltaTime * 2.0f, mNormalizedMouseX );
  mTotalAnimation.step( total_step );
  for( auto &anim : mSpriteAnimations )
  {
    anim.step( deltaTime );
  }
}

void SpriteAnimationApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
  gl::color( Color::white() );
  mSpriteSheet->enableAndBind();
  mTotalAnimation.render();
  for( auto &anim : mSpriteAnimations )
  {
    anim.render();
  }
}

CINDER_APP_NATIVE( SpriteAnimationApp, RendererGl )

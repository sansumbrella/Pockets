#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "pockets/SpriteSheet.h"
#include "pockets/SimpleRenderer.h"
#include "pockets/Sprite.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SpriteRendererApp : public AppNative {
public:
	void setup();
  void mouseMove( MouseEvent event );
  void update();
private:
  pk::SpriteSheetRef      mSpriteSheet;
  pk::SpriteRef           mSprite;
  pk::SpriteAnimationRef  mSpriteAnimation;
  pk::SimpleRendererRef   mRenderer;
  float                   mLastUpdate;
};

void SpriteRendererApp::setup()
{
  mSpriteSheet = pk::SpriteSheet::load( getAssetPath( "sprite_sheet.json" ) );
  auto sprites = mSpriteSheet->getSpriteNames();

  mSprite = pk::SpriteRef( new pk::Sprite( mSpriteSheet->getSpriteData("wood_cracked") ) );
  mSprite->getLocus().setLoc( getWindowSize() / 2 );
  mSprite->setLayer( 2 ); // an arbitrary number > 0 to be in front of other content

  mSpriteAnimation = pk::SpriteAnimationRef( new pk::SpriteAnimation );
  mSpriteAnimation->getLocus().setLoc( getWindowSize() / 2 );
  for( const string &name : sprites )
  {
    mSpriteAnimation->addFrame( mSpriteSheet->getSpriteData(name), 2.0f / 24.0f );
  }

  mRenderer = pk::SimpleRenderer::create();
  mRenderer->add( mSprite );
  mRenderer->add( mSpriteAnimation );
  mRenderer->setPreDrawFn( [=](){ mSpriteSheet->enableAndBind(); } );
  mRenderer->setPostDrawFn( [=](){ mSpriteSheet->unbind(); } );

  getSignalUpdate().connect( [this](){ mRenderer->update(); } );
  getWindow()->getSignalDraw().connect( [this](){
                                        gl::clear( Color::black() );
                                        mRenderer->draw();
                                        } );
  mLastUpdate = getElapsedSeconds();
}

void SpriteRendererApp::mouseMove( MouseEvent event )
{
  mSprite->getLocus().setLoc( event.getPos() );
}

void SpriteRendererApp::update()
{
  float now = getElapsedSeconds();
  float deltaTime = now - mLastUpdate;
  mLastUpdate = now;
  mSpriteAnimation->update( deltaTime );
}

CINDER_APP_NATIVE( SpriteRendererApp, RendererGl )

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
  void draw();
private:
  pk::SpriteSheetRef      mSpriteSheet;
  pk::Sprite              mSprite;
  pk::SpriteAnimationRef  mSpriteAnimation;
  pk::SimpleRendererRef   mRenderer;
  float                   mLastUpdate;
};

void SpriteRendererApp::setup()
{
  mSpriteSheet = pk::SpriteSheet::load( getAssetPath( "sprite_sheet.json" ) );
  auto sprites = mSpriteSheet->getSpriteNames();

  mSprite = mSpriteSheet->getSprite("wood_cracked");
  mSprite.getLocus().setLoc( getWindowSize() / 2 );
  mSprite.pk::SimpleRenderer::IRenderable::setLayer( 2 ); // an arbitrary number > 0 to be in front of other content

  mSpriteAnimation = pk::SpriteAnimationRef( new pk::SpriteAnimation );
  mSpriteAnimation->getLocus().setLoc( getWindowSize() / 2 );
  for( const string &name : sprites )
  {
    mSpriteAnimation->addFrame( mSpriteSheet->getSpriteData(name), 2.0f / 24.0f );
  }

  mRenderer = pk::SimpleRenderer::create();
  mRenderer->add( &mSprite );
  mRenderer->add( mSpriteAnimation.get() );
  // update
  mRenderer->sort();
  mRenderer->setPreDrawFn( [=](){ mSpriteSheet->enableAndBind(); } );
  mRenderer->setPostDrawFn( [=](){ mSpriteSheet->unbind(); } );

  mLastUpdate = getElapsedSeconds();
}

void SpriteRendererApp::mouseMove( MouseEvent event )
{
  mSprite.getLocus().setLoc( event.getPos() );
  mSprite.clipBy( Rectf(getWindowBounds()) / 2 + Vec2f( 100.0f, 100.0f ) );
}

void SpriteRendererApp::update()
{
  float now = getElapsedSeconds();
  float deltaTime = now - mLastUpdate;
  mLastUpdate = now;
  mSpriteAnimation->update( deltaTime );
}

void SpriteRendererApp::draw()
{
  gl::clear( Color::black() );
  gl::color( Color::gray( 0.5f ) );
  gl::drawSolidRect( Rectf(getWindowBounds()) / 2 + Vec2f( 100.0f, 100.0f ) );
  gl::color( Color::white() );
  mRenderer->render();
}

CINDER_APP_NATIVE( SpriteRendererApp, RendererGl )

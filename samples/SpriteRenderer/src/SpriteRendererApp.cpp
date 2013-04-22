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
	void mouseDown( MouseEvent event );	
private:
  pk::SpriteSheetRef    mSpriteSheet;
  pk::SpriteRef         mSprite;
  pk::SimpleRendererRef mRenderer;
};

void SpriteRendererApp::setup()
{
  mSpriteSheet = pk::SpriteSheet::load( getAssetPath( "sprite_sheet.json" ) );

  mSprite = pk::SpriteRef( new pk::Sprite( mSpriteSheet->getSpriteData("wood_cracked") ) );
  mSprite->getLocus().setLoc( getWindowSize() / 2 );
  mSprite->setLayer( 20 );

  mRenderer = pk::SimpleRenderer::create();
  mRenderer->add( mSprite );
  mRenderer->setPreDrawFn( [=](){ mSpriteSheet->enableAndBind(); } );
  mRenderer->setPostDrawFn( [=](){ mSpriteSheet->unbind(); } );

  getSignalUpdate().connect( [this](){ mRenderer->update(); } );
  getWindow()->getSignalDraw().connect( [this](){
                                        gl::clear( Color::black() );
                                        mRenderer->draw();
                                        } );
}

void SpriteRendererApp::mouseDown( MouseEvent event )
{
  mSprite->getLocus().setLoc( event.getPos() );
}

CINDER_APP_NATIVE( SpriteRendererApp, RendererGl )

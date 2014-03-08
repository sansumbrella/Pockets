#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Rand.h"

#include "pockets/cobweb/CobWeb.h"

#include "PhysicsScrolling.h"
#include "TexturePackingSample.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace pockets;


class PocketsApp : public AppNative {
public:
  void prepareSettings( Settings *settings ) override;
	void setup() override;
	void draw() override;
private:
  SceneRef      mCurrentScene;
  cobweb::Node  mRoot;
};

void PocketsApp::prepareSettings( Settings *settings )
{
  settings->enableMultiTouch();
}

void PocketsApp::setup()
{
  mCurrentScene = make_shared<TexturePackingSample>();
  mCurrentScene->setup();
  mCurrentScene->connect( getWindow() );
  mCurrentScene->show( getWindow(), true );

  // TODO: build a CobWeb gui to select between samples
  auto button = make_shared<cobweb::ButtonBase>( getWindowBounds() );
  button->setSelectFn( []() { app::console() << "Hello from button" << endl; } );
  mRoot.appendChild( button );
  mRoot.deepConnect( getWindow() );
  console() << "Button is active: " << (button->isActive() ? "yes" : "no") << endl;
}

void PocketsApp::draw()
{
  mRoot.deepDraw();
}

CINDER_APP_NATIVE( PocketsApp, RendererGl )

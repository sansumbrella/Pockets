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
	void setup();
	void draw();
private:
  SceneRef      mCurrentScene;
};

void PocketsApp::setup()
{
  mCurrentScene = make_shared<TexturePackingSample>();
  mCurrentScene->setup();
  mCurrentScene->connect( getWindow() );
  mCurrentScene->show( getWindow(), true );

  // TODO: build a CobWeb gui to select between samples
}

void PocketsApp::draw()
{
}

CINDER_APP_NATIVE( PocketsApp, RendererGl )

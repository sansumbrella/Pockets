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
  void nextSample();
private:
  vector<std::function<SceneRef ()> > mConstructors;
  SceneRef      mCurrentScene;
  size_t        mIndex = 0;
  cobweb::Node  mRoot;
};

void PocketsApp::prepareSettings( Settings *settings )
{
}

void PocketsApp::setup()
{
  mConstructors.push_back( &make_shared<PhysicsScrolling> );
  mConstructors.push_back( &make_shared<TexturePackingSample> );


  // TODO: build a CobWeb gui to select between samples
  auto button = make_shared<cobweb::ButtonBase>( Rectf( 0.0f, 0.0f, 100.0f, 100.0f ) );
  button->setSelectFn( [this]() { nextSample(); } );
  mRoot.appendChild( button );
  mRoot.deepConnect( getWindow() );
  console() << "Button is active: " << (button->isActive() ? "yes" : "no") << endl;

  nextSample();
}

void PocketsApp::nextSample()
{
  mCurrentScene = mConstructors[mIndex]();
  mCurrentScene->setup();
  mCurrentScene->connect( getWindow() );
  mCurrentScene->show( getWindow(), true );
  mIndex++;
  if( mIndex >= mConstructors.size() ){ mIndex = 0; }
}

void PocketsApp::draw()
{
  mRoot.deepDraw();
}

CINDER_APP_NATIVE( PocketsApp, RendererGl )

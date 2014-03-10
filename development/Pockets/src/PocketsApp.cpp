#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Font.h"

#include "cinder/Rand.h"

#include "pockets/cobweb/CobWeb.h"

#include "PhysicsScrolling.h"
#include "TexturePackingSample.h"

#include "glm.hpp"

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
  Font arial( "Arial", 14.0f );
  auto button = cobweb::SimpleButton::createLabelButton( "Next Sample", arial );

  button->setSelectFn( [this]() { nextSample(); } );
  button->setPosition( getWindowSize() - button->getSize() );
  mRoot.appendChild( button );
  mRoot.deepConnect( getWindow() );

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

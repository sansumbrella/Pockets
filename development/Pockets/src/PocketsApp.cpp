#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Font.h"

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
  void nextScene();
  void playScene( const function<SceneRef ()> &builder );
  void buildMenu();
private:
  vector< pair<string, function<SceneRef ()> > > mScenes;
  SceneRef      mCurrentScene;
  size_t        mIndex = 0;
  cobweb::Node  mRoot;
};

void PocketsApp::prepareSettings( Settings *settings )
{
  settings->setWindowSize( 1280, 720 );
}

void PocketsApp::setup()
{
  mScenes.push_back( make_pair( "Physics Scrolling Thing", &make_shared<PhysicsScrolling> ) );
  mScenes.push_back( make_pair( "Texture Packer", &make_shared<TexturePackingSample> ) );
  mScenes.push_back( make_pair( "Sprite Animation", &make_shared<Scene> ) );
  mScenes.push_back( make_pair( "Simple GUI", &make_shared<Scene> ) );

  buildMenu();
}

void PocketsApp::playScene(const function<SceneRef ()> &builder)
{
  mCurrentScene = builder();
  mCurrentScene->setup();
  mCurrentScene->connect( getWindow() );
  mCurrentScene->show( getWindow(), true );
}

void PocketsApp::nextScene()
{
  if( ++mIndex >= mScenes.size() ){ mIndex = 0; }
  playScene( mScenes[mIndex].second );
}

void PocketsApp::draw()
{
  mRoot.deepDraw();
}

void PocketsApp::buildMenu()
{
  Font arial( "Arial", 14.0f );
  int y = 0;
  size_t index = 0;
  int widest = 0;
  for( auto &pair : mScenes )
  {
    auto button = cobweb::SimpleButton::createLabelButton( pair.first, arial );
    mRoot.appendChild( button );
    auto fn = pair.second;
    button->setSelectFn( [this,index,fn]() { mIndex = index; playScene( fn ); } );
    button->setPosition( Vec2f( 0.0f, y ) );
    y += button->getHeight() + 2;
    widest = math<float>::max( button->getWidth(), widest );
    index++;
  }

  auto button = cobweb::SimpleButton::createLabelButton( "Next Scene", arial );
  button->setPosition( Vec2f( 0.0f, y ) );
  button->setSelectFn( [this](){ nextScene(); } );
  mRoot.appendChild( button );
  mRoot.setPosition( Vec2f( getWindowWidth() - (widest + 10.0f), 10.0f ) );
  mRoot.connectRoot( getWindow() );
}

CINDER_APP_NATIVE( PocketsApp, RendererGl )

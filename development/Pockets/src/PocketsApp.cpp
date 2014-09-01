#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Font.h"

#include "cinder/Rand.h"

#include "pockets/cobweb/CobWeb.h"

#include "PhysicsScrolling.h"
#include "TexturePackingSample.h"
#include "TreentSample.h"
#include "TreentGui.h"
#include "ChoreographSample.h"

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
  void prevScene();
  void playScene( const function<SceneRef ()> &builder );
  void buildMenu();
private:
  vector< pair<string, function<SceneRef ()> > > mScenes;
  SceneRef          mCurrentScene;
  int               mIndex = 0;
  cobweb::RootNode  mRoot;
};

void PocketsApp::prepareSettings( Settings *settings )
{
  settings->setWindowSize( 1280, 720 );
  settings->enableMultiTouch();
  settings->disableFrameRate();
}

void PocketsApp::setup()
{
  gl::enableVerticalSync();

  mScenes.push_back( make_pair( "Choreograph Placeholder", &make_shared<ChoreographSample> ) );
  mScenes.push_back( make_pair( "Physics Scrolling Thing", &make_shared<PhysicsScrolling> ) );
  // Rotating things.
  mScenes.push_back( make_pair( "Treant Test", &make_shared<TreentTest> ) );
  // In progress: buttons, a scroll panel, a slider.
  mScenes.push_back( make_pair( "Treent GUI", &make_shared<TreentGui> ) );
  // Pack a stack of textures into a sprite sheet
  mScenes.push_back( make_pair( "Texture Packer", &make_shared<TexturePackingSample> ) );
  // Load an SVG file and turn layers into sprites
  mScenes.push_back( make_pair( "SVG Spritifier", &make_shared<TexturePackingSample> ) );
  mScenes.push_back( make_pair( "Sprite Animation", &make_shared<Scene> ) );

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
  if( ++mIndex >= mScenes.size() ) { mIndex = 0; }
  playScene( mScenes[mIndex].second );
}

void PocketsApp::prevScene()
{
  if( --mIndex < 0 ) { mIndex = mScenes.size() - 1; }
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
    button->setPosition( vec2( 0.0f, y ) );
    y += button->getHeight() + 2;
    widest = math<float>::max( button->getWidth(), widest );
    index++;
  }

  auto prev = cobweb::SimpleButton::createLabelButton( "Previous", arial );
  prev->setPosition( vec2( 0.0f, y ) );
  prev->setSelectFn( [this](){ prevScene(); } );
  auto next = cobweb::SimpleButton::createLabelButton( "Next", arial );
  next->setPosition( vec2( prev->getWidth() + 2, y ) );
  next->setSelectFn( [this](){ nextScene(); } );
  widest = math<float>::max( next->getWidth() + next->getPosition().x, widest );

  mRoot.appendChild( prev );
  mRoot.appendChild( next );

  mRoot.setPosition( vec2( getWindowWidth() - (widest + 10.0f), 10.0f ) );
  mRoot.connect( getWindow() );
}

CINDER_APP_NATIVE( PocketsApp, RendererGl )

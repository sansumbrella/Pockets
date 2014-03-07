#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Rand.h"

#include "pockets/SimplePhysics.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace pockets;


class PocketsApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );
  void mouseDrag( MouseEvent event );
  void mouseUp( MouseEvent event );
	void update();
	void draw();
private:

  physics::World    mWorld;
  Vec2f             mMousePos;
  Vec2f             mMouseStart;
  Vec2f             mNodeStart;
  bool              mMouseDown = false;

  physics::NodeRef  mActualPosition; // connected to target by spring, draw here
  physics::NodeRef  mTargetPosition; // moves with/flung by user
};

void PocketsApp::setup()
{

  Rand r;

  mWorld.friction( 0.5f );
  mActualPosition = mWorld.createNode( getWindowCenter() );
  mTargetPosition = mWorld.createNode( getWindowCenter() );
  mWorld.createConstraint<physics::Leash>( mActualPosition, mTargetPosition, 0.05f );
  float centerX = getWindowWidth() / 2.0f;
  mWorld.createConstraint<physics::Range>( mTargetPosition, Vec2f( centerX, 0.0f ), Vec2f( centerX, getWindowHeight() - 200.0f ) );
  mWorld.createConstraint<physics::Range>( mActualPosition, Vec2f( centerX, -100.0f ), Vec2f( centerX, getWindowHeight() - 100.0f ) );
}

void PocketsApp::mouseDown( MouseEvent event )
{
  mMouseDown = true;
  mMousePos = event.getPos();
  mMouseStart = event.getPos();
  mNodeStart = mTargetPosition->pos;
}

void PocketsApp::mouseDrag( MouseEvent event )
{
  mMousePos = event.getPos();
}

void PocketsApp::mouseUp( MouseEvent event )
{
  mMouseDown = false;
  mMousePos = event.getPos();
  auto pos = mNodeStart + (mMousePos - mMouseStart);
  mTargetPosition->pos = pos;
}

void PocketsApp::update()
{
  if( mMouseDown )
  {
    auto pos = mNodeStart + (mMousePos - mMouseStart);
    mTargetPosition->pos = mTargetPosition->ppos = pos;
  }
  mWorld.step( 1.0 / 60.0 );
}

void PocketsApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );

  gl::color( Color( 1.0f, 1.0f, 0.0f ) );
  gl::drawSolidRect( Rectf( mActualPosition->pos, mActualPosition->pos + Vec2f( 100.0f, 200.0f ) ) );
  gl::color( Color( 1.0f, 0.0f, 1.0f ) );
  gl::drawStrokedCircle( mTargetPosition->pos, 12.0f );
}

CINDER_APP_NATIVE( PocketsApp, RendererGl )

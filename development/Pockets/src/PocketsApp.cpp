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
  physics::NodeRef  mMouseNode;
  Vec2f             mMousePos;
};

void PocketsApp::setup()
{

  Rand r;

  mWorld.friction( 0.4f );
  auto root = mWorld.createNode();
  root->ppos = root->pos = getWindowCenter();
  physics::NodeRef prev;

  for( int i = 0; i < 100; ++i )
  {
    auto node = mWorld.createNode();
    node->pos = node->ppos = getWindowCenter() + r.nextVec2f() * 100.0f;
//    node->ppos += r.nextVec2f() * 10.0f;
    mWorld.createConstraint<physics::Spring>( root, node, 0.25f );
    if( prev )
    { mWorld.createConstraint<physics::Spring>( prev, node )->stiffness( 0.5f ); }
    prev = node;
  }


}

void PocketsApp::mouseDown( MouseEvent event )
{
  mMousePos = event.getPos();
  mMouseNode = mWorld.nearestNode( event.getPos() );
  mMouseNode->ppos = mMouseNode->pos = event.getPos();
}

void PocketsApp::mouseDrag( MouseEvent event )
{
  if( mMouseNode )
  { mMouseNode->ppos = mMouseNode->pos = event.getPos(); }
  mMousePos = event.getPos();
}

void PocketsApp::mouseUp( MouseEvent event )
{
  if( mMouseNode )
  {
    mMouseNode->pos = event.getPos();
    mMouseNode.reset();
  }
}

void PocketsApp::update()
{
  mWorld.step( 1.0 / 60.0 );
  if( mMouseNode )
  {
    mMouseNode->pos = mMouseNode->ppos = mMousePos;
  }
}

void PocketsApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
  mWorld.drawNodes();
}

CINDER_APP_NATIVE( PocketsApp, RendererGl )

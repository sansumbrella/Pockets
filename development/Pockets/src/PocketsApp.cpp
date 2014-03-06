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
	void update();
	void draw();
private:

  physics::World  mWorld;
};

void PocketsApp::setup()
{

  Rand r;
  for( int i = 0; i < 100; ++i )
  {
    auto node = mWorld.createNode();
    node->pos = node->ppos = Vec3f( getWindowWidth()/2.0f, getWindowHeight()/2.0f, 0.0f ) + r.nextVec3f() * 100.0f;
    node->ppos += r.nextVec3f() * 10.0f;
  }


}

void PocketsApp::mouseDown( MouseEvent event )
{
}

void PocketsApp::update()
{
  mWorld.step( 1.0 / 60.0 );
}

void PocketsApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
  mWorld.drawNodes();
}

CINDER_APP_NATIVE( PocketsApp, RendererGl )

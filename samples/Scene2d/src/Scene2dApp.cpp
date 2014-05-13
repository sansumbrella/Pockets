#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "pockets/scene2d/Node.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace pockets;

/**

 Basic and moderately advanced use of the scene2d UI building tool
 
 // connect the root ui element to events

*/

class Scene2dApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
private:
  NodeRef mRootNode;
};

void Scene2dApp::setup()
{
  mRootNode = Node::create();
}

void Scene2dApp::mouseDown( MouseEvent event )
{
}

void Scene2dApp::update()
{
}

void Scene2dApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
}

CINDER_APP_NATIVE( Scene2dApp, RendererGl )

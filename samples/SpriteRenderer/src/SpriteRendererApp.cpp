#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SpriteRendererApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void SpriteRendererApp::setup()
{
}

void SpriteRendererApp::mouseDown( MouseEvent event )
{
}

void SpriteRendererApp::update()
{
}

void SpriteRendererApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( SpriteRendererApp, RendererGl )

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "TriangleRenderer.h"
#include "SimpleRenderer.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace pockets;

class Box : public TriangleRenderer::IRenderable, public SimpleRenderer::IRenderable
{

};

typedef function<void ()> RenderFn;

class RendererTestApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
private:
  RenderFn mRenderFn;
  TriangleRenderer  mTriangleRenderer;
  SimpleRenderer    mSimpleRenderer;
};

void RendererTestApp::setup()
{
}

void RendererTestApp::mouseDown( MouseEvent event )
{
}

void RendererTestApp::update()
{
}

void RendererTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( RendererTestApp, RendererGl )

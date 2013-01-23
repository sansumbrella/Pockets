#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"

#include "pockets/gl/Vbo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct VertexData
{
  Vec3f	position;
  Vec3f normal;
  ColorA	color;
  Vec2f texcoord;
};

class VboSampleApp : public AppCocoaTouch {
  public:
	virtual void	setup();
	virtual void	update();
	virtual void	draw();
	virtual void	mouseDown( MouseEvent event );
		
	Matrix44f	mCubeRotation;
	gl::Texture mTex;
	CameraPersp	mCam;
};

void VboSampleApp::setup()
{
	mCubeRotation.setToIdentity();

	// Create a blue-green gradient as an OpenGL texture
	Surface8u surface( 256, 256, false );
	Surface8u::Iter iter = surface.getIter();
	while( iter.line() ) {
		while( iter.pixel() ) {
			iter.r() = 0;
			iter.g() = iter.x();
			iter.b() = iter.y();
		}
	}
	
	mTex = gl::Texture( surface );

  pockets::gl::AttributeSet attributes;
  attributes.addAttribute( "position", 3, GL_FLOAT, offsetof( VertexData, position ) );
  attributes.addAttribute( "normal", 3, GL_FLOAT, offsetof( VertexData, normal ) );
  attributes.addAttribute( "color", 3, GL_FLOAT, offsetof( VertexData, color ) );
  attributes.addAttribute( "texcoord", 2, GL_FLOAT, offsetof( VertexData, texcoord ) );
  attributes.setStride( sizeof( VertexData ) );

  for( auto &attr : attributes )
  {
    cout << "Offset of: " << attr.getName() << " = " << attr.getOffset() << endl;
  }

  auto indices = pockets::gl::Vbo::create( pockets::gl::Attribute( "index", 1, GL_UNSIGNED_SHORT ) );
  auto vertices = pockets::gl::Vbo::create( attributes );
  auto mesh = pockets::gl::VboMesh::create();
  mesh->addBuffer( indices );
  mesh->addBuffer( vertices );
}

void VboSampleApp::mouseDown( MouseEvent event )
{
	console() << "Mouse down @ " << event.getPos() << std::endl;
}

void VboSampleApp::update()
{
	mCubeRotation.rotate( Vec3f( 1, 1, 1 ), 0.03f );
}

void VboSampleApp::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.3f ) );
	gl::enable( GL_TEXTURE_2D );
	gl::enableDepthRead();
	
	mTex.bind();
  gl::draw( mTex );
}

CINDER_APP_COCOA_TOUCH( VboSampleApp, RendererGl )

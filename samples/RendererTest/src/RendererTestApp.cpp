#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "TriangleRenderer.h"
#include "SimpleRenderer.h"
#include "Locus2d.h"

#include <array>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace pockets;

/**
 Test for comparing performance of rendering strategies.
 Also demonstrates basic usage of the Renderer objects.

 Performance notes from testing on various devices (Release mode, 1k boxes):

 (Renderer/Device)        SimpleRenderer    TriangleRenderer
 iPhone 3GS               ~ 55ms             ~ 10ms
 iPhone 4S                ~ 25.5ms           ~  5.5ms
 iPad 2                   ~ 21.8ms           ~  6.6ms
 iPad 3 (Retina)          ~ 12.5ms           ~  4.6ms
 Macbook Pro              ~  2.7ms           ~  1.5ms

 Performance seems to increase much more on devices with limited GPUs, where
 GPU state switching is slow. This was all measured while plugged into XCode.

 SimpleRenderer, then, provides an easy interface for ordering custom rendering,
 while TriangleRenderer provides a high performance interface for rendering
 triangle strips.
 */

// Something we can render with either renderer for better comparison of methods
class Box : public TriangleRenderer::IRenderable, public SimpleRenderer::IRenderable
{
public:
  Box()
  { // build a box
    Rectf bounds{ -20.0f, -10.0f, 20.0f, 10.0f };
    mVertices[0].position = bounds.getUpperRight();
    mVertices[1].position = bounds.getUpperLeft();
    mVertices[2].position = bounds.getLowerRight();
    mVertices[3].position = bounds.getLowerLeft();
  }
  // accessors and mutators
  Vec2f getPos() const { return mLocus.getLoc(); }
  void setPos( const Vec2f &pos ){ mLocus.setLoc( pos ); }
  float getRotation() const { return mLocus.getRotation(); }
  void setRotation( float radians ){ mLocus.setRotation( radians ); }
  void setColor( const ColorA &color )
  {
    mColor = color;
    for( auto &v : mVertices )
    {
      v.color = color;
    }
  }
  // Interface for SimpleRenderer
  void render() override
  {
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 2, GL_FLOAT, sizeof(Vertex), &mVertices[0].position.x );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), &mVertices[0].tex_coord.x );

    gl::pushModelView();
    gl::multModelView( mLocus );
    gl::color( mColor );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, mVertices.size() );
    gl::popModelView();

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  }
  // Interface for TriangleRenderer
  vector<Vertex> getVertices() override
  {
    auto mat = mLocus.getTransform();
    vector<Vertex> ret;
    for( auto &v : mVertices )
    {
      ret.emplace_back( Vertex{ mat.transformPoint( v.position ), v.color, v.tex_coord } );
    }
    return ret;
  }
private:
  ci::ColorA                mColor;
  std::array<Vertex, 4>     mVertices;
  Locus2d                   mLocus;
};

typedef function<void ()> RenderFn;

class RendererTestApp : public AppNative {
public:
	void setup();
	void swapRenderer();
	void update();
	void draw();
private:
  RenderFn          mRenderFn;
  RenderFn          mRenderTriangle;
  RenderFn          mRenderSimple;
  TriangleRenderer  mTriangleRenderer;
  SimpleRenderer    mSimpleRenderer;
  array<Box, 1000>  mBoxes;
  bool              mRenderingSimple = true;
  double            mAverageRenderTime = 0;
};

void RendererTestApp::setup()
{
  mRenderTriangle = [=](){ mTriangleRenderer.render(); };
  mRenderSimple = [=](){ mSimpleRenderer.render(); };
  mRenderFn = mRenderSimple;

  for( auto &box : mBoxes )
  {
    box.setColor( ColorA{ CM_HSV, Rand::randFloat(), 0.9f, 0.9f, 1.0f } );
    box.setPos( Vec2f{ Rand::randFloat(getWindowWidth()), Rand::randFloat(getWindowHeight()) } );
    box.setRotation( Rand::randFloat( M_PI * 2 ) );
    mTriangleRenderer.add( &box );
    mSimpleRenderer.add( &box );
  }

  // We perform the cast since we know what type of things we stored in each renderer
  // A type-safe way could be to assign y to each objects layer and then sort by layer
  Vec2f center = getWindowCenter();
  auto vortex_simple = [center]( const SimpleRenderer::IRenderable *lhs, const SimpleRenderer::IRenderable *rhs )
  {
    return static_cast<const Box*>( lhs )->getPos().distance(center) <
    static_cast<const Box*>( rhs )->getPos().distance(center);
  };
  auto vortex_triangle = [center]( const TriangleRenderer::IRenderable *lhs, const TriangleRenderer::IRenderable *rhs )
  {
    return  static_cast<const Box*>( lhs )->getPos().distance(center) <
    static_cast<const Box*>( rhs )->getPos().distance(center);
  };
  mSimpleRenderer.sort( vortex_simple );
  mTriangleRenderer.sort( vortex_triangle );

  getWindow()->getSignalKeyUp().connect( [this](KeyEvent &event){ swapRenderer(); } );
  getWindow()->getSignalTouchesEnded().connect( [this](TouchEvent &event){ swapRenderer(); } );
}

void RendererTestApp::swapRenderer()
{
  cout << (mRenderingSimple ? "SimpleRenderer " : "TriangleRenderer ")
  << "avg render time: " << mAverageRenderTime << "ms" << endl;
  cout << "Swapping renderer" << endl;
  mRenderingSimple = !mRenderingSimple;
  mRenderFn = mRenderingSimple ? mRenderSimple : mRenderTriangle;
}

void RendererTestApp::update()
{
  for( auto &box : mBoxes )
  {
    box.setRotation( fmodf( box.getRotation() + M_PI * 0.01f, M_PI * 2 ) );
  }
}

void RendererTestApp::draw()
{
	gl::clear( Color::black() );
  gl::color( Color::white() );
  auto t1 = getElapsedSeconds();
  mRenderFn();
  auto t2 = getElapsedSeconds();
  auto ms = (t2 - t1) * 1000;
  mAverageRenderTime = (mAverageRenderTime * 59 + ms) / 60;
  if( getElapsedFrames() % 120 == 0 )
  {
    cout << (mRenderingSimple ? "SimpleRenderer " : "TriangleRenderer ")
    << "avg render time: " << mAverageRenderTime << "ms" << endl;
  }
}

CINDER_APP_NATIVE( RendererTestApp, RendererGl( RendererGl::AA_NONE ) )

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "pockets/ExpandedLine2d.h"
#include "pockets/ExpandedPath2d.h"
#include "pockets/ExpandedPath3d.h"
#include "pockets/LineUtils.h"

#include "cinder/Rand.h"
#include "cinder/Easing.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace pockets;

class PathStrokeRibbonTubeApp : public AppNative {
public:
	void setup();
	void update();
	void draw();
  void mouseMove( MouseEvent event ) override;
private:
  ExpandedLine2dRef mLine2d;
  ExpandedPath2dRef mPath2d;
  ExpandedPath3dRef mPath3d;
};

void PathStrokeRibbonTubeApp::setup()
{
  // Create an expanded line between two points
  mLine2d = ExpandedLine2d::create( Vec2f{ 50.0f, 100.0f }, Vec2f{ 200.0f, 50.0f } );
  mLine2d->setWidth( 20.0f );

  // Create an expanded path passing through many points
  mPath2d = ExpandedPath2d::create( 20 );
  vector<Vec2f> path2d_positions;
  path2d_positions.assign( 20, getWindowCenter() );
  mPath2d->setPositions( path2d_positions );

  // Create an expanded path passing through many points in 3d
  Rand r{};
  const size_t curve_length = 8;
  vector<Vec3f> curve_positions;
  for( int i = 0; i < curve_length; ++i )
  {
    curve_positions.push_back( Vec3f{ r.nextFloat( getWindowWidth() ), r.nextFloat( getWindowHeight() ), r.nextFloat( 0.0f, -100.0f ) } );
  }
  BSpline3f curve{ curveThrough( curve_positions ), 3, false, true };
  // TODO: adaptive resampling of the curve
  // for now, just blindly sample the curve
  const size_t curve_samples = 200;
  vector<Vec3f> sample_positions;
  for( int i = 0; i < curve_samples; ++i )
  {
    sample_positions.push_back( curve.getPosition( i / (curve_samples - 1.0f) ) );
  }
  mPath3d = ExpandedPath3d::create( curve_samples );
  mPath3d->setWidth( 16.0f );
  mPath3d->setShapeFn( []( float t ) {
    if( t < 0.5f )
    { // taper one end
      return easeInOutQuart( t * 2.0f );
    }
    return 1.0f;
  } );
  mPath3d->setPositions( sample_positions );
}

void PathStrokeRibbonTubeApp::mouseMove( MouseEvent event )
{
  mPath2d->shiftFront( event.getPos() );
}

void PathStrokeRibbonTubeApp::update()
{
}

void PathStrokeRibbonTubeApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
  gl::setMatricesWindowPersp( getWindowSize() );
  gl::color( Color::white() );
  mLine2d->render();
  gl::color( Color( 1.0f, 0.0f, 0.0f ) );
  mPath3d->draw();
  gl::color( Color( 1.0f, 1.0f, 0.0f ) );
  mPath2d->draw();
}

CINDER_APP_NATIVE( PathStrokeRibbonTubeApp, RendererGl )

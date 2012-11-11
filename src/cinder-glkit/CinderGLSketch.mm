#include "CinderGLSketch.h"

#include "cinder/Utilities.h"
#include "cinder/app/App.h"

#include "SnibbeDevice.h"

using namespace cinder;

// statics

CinderGLSketch  *CinderGLSketch::gSketch = 0;
    
//
// static
CinderGLSketch * CinderGLSketch::sketch()
{
    return gSketch;
}
    
void CinderGLSketch::privateSetup__()
{
    mTimeline->stepTo(getElapsedSeconds());
    setup();
}

void CinderGLSketch::privateUpdate__()
{
    mTimeline->stepTo(getElapsedSeconds());
    update();
    mFrameCount++;
}



//
//
void CinderGLSketch::touchesBegan( app::TouchEvent event )
{
    bool handled = false;
    for( CallbackMgr<bool (TouchEvent)>::iterator cbIter = mCallbacksTouchesBegan.begin(); ( cbIter != mCallbacksTouchesBegan.end() ) && ( ! handled ); ++cbIter )
    {
        handled = (cbIter->second)( event );
    }
    
}

//
//
void CinderGLSketch::touchesMoved( app::TouchEvent event )
{
    bool handled = false;
    for( CallbackMgr<bool (TouchEvent)>::iterator cbIter = mCallbacksTouchesMoved.begin(); ( cbIter != mCallbacksTouchesMoved.end() ) && ( ! handled ); ++cbIter )
    {
        handled = (cbIter->second)( event );
    }
   
}

//
//
void CinderGLSketch::touchesEnded( app::TouchEvent event )
{
    bool handled = false;
    for( CallbackMgr<bool (TouchEvent)>::iterator cbIter = mCallbacksTouchesEnded.begin(); ( cbIter != mCallbacksTouchesEnded.end() ) && ( ! handled ); ++cbIter )
    {
        handled = (cbIter->second)( event );
    }
    
}

//
// virtual
void CinderGLSketch::touchesCanceled(app::TouchEvent event)
{
    touchesEnded( event );
}

//
// virtual
void CinderGLSketch::onGesturePinch( float factor )
{
    for( CallbackMgr<void (float)>::iterator cbIter = mCallbacksGesturePinch.begin(); cbIter != mCallbacksGesturePinch.end(); ++cbIter )
    {
        (cbIter->second)( factor );
    }
    
}

std::string CinderGLSketch::getDeviceString() const
{
	std::string platformString = [[SnibbeDevice platformString] UTF8String];
	return platformString;
}

CISketch * cinder::getSketch()
{
    return CinderGLSketch::sketch();
}

    
    
std::ostream& cinder::console()
{
    return ci::getSketch()->console();
}
    


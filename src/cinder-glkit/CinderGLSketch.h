#pragma once

#include "Cinder.h"
#include "Vector.h"
#include "Area.h"
#include "Timer.h"
#include "Timeline.h"
#include "TouchEvent.h"

#include <ostream>


namespace cinder {

using app::TouchEvent;
    
class CinderGLSketch {
public:

    enum UIInterfaceOrientation {
        UIInterfaceOrientationPortrait           = 1,
        UIInterfaceOrientationPortraitUpsideDown = 2,
        UIInterfaceOrientationLandscapeRight     = 3,
        UIInterfaceOrientationLandscapeLeft      = 4
    };

    CinderGLSketch()
    : mNeedsSetup(true)
    , mTimer(true)
    , mTimeline(Timeline::create())
    , mFrameCount(0)
    , mContentScaleFactor_(1)
    {
        gSketch = this;
    };
    
    virtual ~CinderGLSketch(){};

    void privateSetup__();
    void privateUpdate__();

    virtual void setup(){}    
    virtual void tearDown(){}
    virtual void update(){}
    virtual void draw(const Area &area){}
    
    Timeline& timeline(){ return *mTimeline; }
    
    const Vec2i& getSize() const { return mSize; }
    void         setSize(const Vec2i &size) { mSize = size; }
    Area         getBounds() const { return Area(0, 0, mSize.x, mSize.y); }
    float        getAspectRatio() const { return (float)mSize.x / (float)mSize.y; }
	std::string  getDeviceString() const;
    
    // dgm - added to help with retina calculations
    void         setContentScaleFactor( float scaleFactor ) { mContentScaleFactor_ = scaleFactor; }
    float        contentScaleFactor() const { return mContentScaleFactor_; }
    const Vec2i  logicalSize() const { return mSize / mContentScaleFactor_; }
    
    double getElapsedSeconds() const { return mTimer.getSeconds(); }
    uint32_t getElapsedFrames() const { return mFrameCount; }

    virtual void touchesBegan(app::TouchEvent event);
    virtual void touchesMoved(app::TouchEvent event);
    virtual void touchesEnded(app::TouchEvent event);
    virtual void touchesCanceled(app::TouchEvent event);
    
    virtual void onGesturePinch( float factor );
    
    virtual bool supportsInterfaceOrientation(UIInterfaceOrientation orientation){ return true; }
        
    
	//! Registers a callback for touchesBegan events. Returns a unique identifier which can be used as a parameter to unregisterTouchesBegan().
	CallbackId		registerTouchesBegan( std::function<bool (TouchEvent)> callback ) { return mCallbacksTouchesBegan.registerCb( callback ); }
    
	//! Registers a callback for touchesBegan events. Returns a unique identifier which can be used as a parameter to unregisterTouchesBegan().
	template<typename T>
	CallbackId		registerTouchesBegan( T *obj, bool (T::*callback)(TouchEvent) ) { return mCallbacksTouchesBegan.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
	
    //! Unregisters a callback for touchesBegan events.
	void			unregisterTouchesBegan( CallbackId id ) { mCallbacksTouchesBegan.unregisterCb( id ); }
    
	//! Registers a callback for touchesMoved events. Returns a unique identifier which can be used as a parameter to unregisterTouchesMoved().
	CallbackId		registerTouchesMoved( std::function<bool (TouchEvent)> callback ) { return mCallbacksTouchesMoved.registerCb( callback ); }
	
    //! Registers a callback for touchesMoved events. Returns a unique identifier which can be used as a parameter to unregisterTouchesMoved().
	template<typename T>
	CallbackId		registerTouchesMoved( T *obj, bool (T::*callback)(TouchEvent) ) { return mCallbacksTouchesMoved.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
	
    //! Unregisters a callback for touchesMoved events.
	void			unregisterTouchesMoved( CallbackId id ) { mCallbacksTouchesMoved.unregisterCb( id ); }
    
	//! Registers a callback for touchesEnded events. Returns a unique identifier which can be used as a parameter to unregisterTouchesEnded().
	CallbackId		registerTouchesEnded( std::function<bool (TouchEvent)> callback ) { return mCallbacksTouchesEnded.registerCb( callback ); }
	
    //! Registers a callback for touchesEnded events. Returns a unique identifier which can be used as a parameter to unregisterTouchesEnded().
	template<typename T>
	CallbackId		registerTouchesEnded( T *obj, bool (T::*callback)(TouchEvent) ) { return mCallbacksTouchesEnded.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
	
    //! Unregisters a callback for touchesEnded events.
	void			unregisterTouchesEnded( CallbackId id ) { mCallbacksTouchesEnded.unregisterCb( id ); }
    
    //! Registers a callback for pinch gestures if supported
	template<typename T>
	CallbackId		registerPinchGesture( T *obj, void (T::*callback)(float) ) { return mCallbacksGesturePinch.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
	
    //! Unregisters a callback for pinch gestures events.
	void			unregisterPinchGesture( CallbackId id ) { mCallbacksGesturePinch.unregisterCb( id ); }
    
    
    const std::vector<app::TouchEvent::Touch>& getActiveTouches() const { return mActiveTouches; }
    void setActiveTouches(std::vector<app::TouchEvent::Touch> touches){ mActiveTouches = touches; }
    
    std::ostream& console(){ return std::cout; }
    
    bool mNeedsSetup;

    static CinderGLSketch * sketch();
    
protected:

    static CinderGLSketch * gSketch; // singleton
    
    Vec2i mSize;
    float mContentScaleFactor_;
    
    Timer mTimer;
    TimelineRef mTimeline;
    
    uint32_t mFrameCount;
    
    std::vector< app::TouchEvent::Touch > mActiveTouches;

    CallbackMgr<bool (TouchEvent)>		mCallbacksTouchesBegan, mCallbacksTouchesMoved, mCallbacksTouchesEnded;
    CallbackMgr<void (float)>		mCallbacksGesturePinch;
    
	//CallbackMgr<bool (AccelEvent)>		mCallbacksAccelerated;
    
};

// helpers
typedef CinderGLSketch CISketch;
CISketch * getSketch();

std::ostream& console();
    

} // namespace cinder
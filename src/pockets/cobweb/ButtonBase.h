//
//  ButtonBase.h
//  WordShift
//
//  Created by David Wicks on 4/24/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "Node.h"


namespace pockets
{
  namespace cobweb
  {
  typedef std::shared_ptr<class ButtonBase> ButtonRef;
  /**
   Handler of touch events to fire off button "presses"
   Doesn't render itself

   Subclasses are responsible for drawing themselves to screen.
   Would be cool to have a button decorator to turn anything into a button
  */
  class ButtonBase : public Node
  {
  public:
    typedef std::function<void ()> SelectFn;
    ButtonBase( const ci::Rectf &bounds );
    virtual ~ButtonBase();
    void            connect( ci::app::WindowRef window );
    void            customDisconnect(){ mActive = false; }
    //! returns hit box size in points
    ci::Rectf     	getHitBounds() const { return mHitBounds; }
    //! set the hit bounds in points
    void            setHitBounds( const ci::Rectf &bounds ){ mHitBounds = bounds; }
    //! expand hit box by \a horizontal and \a vertical pixels on each side
    void            expandHitBounds( float horizontal, float vertical );
    //! set the function to call when this button is selected or "pressed"
    void            setSelectFn( SelectFn fn ){ mSelectFn = fn; }
    void            emitSelect(){ if( mSelectFn ){ mSelectFn(); } }
    //! stop tracking the touch
    void            cancelInteractions();
    //! called when a finger enters the button's hit area
    virtual void    hoverStart(){}
    //! called when a finger leaves the button's hit area
    virtual void    hoverEnd(){}
    //! true if we are connected to a window's events
    bool            isActive(){ return mActive; }
  private:
    //! touch boundary of element
    ci::Rectf           mHitBounds;
    uint32_t            mTrackedTouch = 0;
    SelectFn            mSelectFn;
    std::atomic<bool>   mActive = { false };
    bool                mHovering = false;
    //! touch event handlers (called when connected to a window)
    void            touchesBegan( ci::app::TouchEvent &event );
    void            touchesMoved( ci::app::TouchEvent &event );
    void            touchesEnded( ci::app::TouchEvent &event );

    void setHovering();
    void endHovering( bool selected );

    //! returns whether a point is inside the button's hit box
    bool            contains( const ci::Vec2i point ) { return mHitBounds.contains( transformedPoint(point) ); }
    //! unproject point from our drawing space
    ci::Vec2f       transformedPoint( const ci::Vec2f &point ) { return getLocus()->toMatrix().invertCopy().transformPoint( point ); }
  };
  } // cobweb::
} // pockets::

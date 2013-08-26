//
//  ScrollPane.cpp
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "ScrollPane.h"

using namespace cascade;
using namespace cinder;
using namespace std;

ScrollPane::ScrollPane( const Vec2i &size ):
Node( size )
, mViewport( 0, 0, app::toPixels(size.x), app::toPixels(size.y) )
, mDrawBounds( app::toPoints( mViewport ) )
{
  mScrollParent->setParent( getLocus() );
  mTimer.start();
}

ScrollPane::~ScrollPane()
{}

void ScrollPane::childAdded(NodeRef element)
{
  element->getLocus()->setParent( mScrollParent );
}

void ScrollPane::connect(ci::app::WindowRef window)
{
  storeConnection( window->getSignalTouchesBegan().connect( [this](app::TouchEvent &event){ touchesBegan( event ); } ) );
  storeConnection( window->getSignalTouchesMoved().connect( [this](app::TouchEvent &event){ touchesMoved( event ); } ) );
  storeConnection( window->getSignalTouchesEnded().connect( [this](app::TouchEvent &event){ touchesEnded( event ); } ) );
}

void ScrollPane::touchesBegan(ci::app::TouchEvent &event)
{
  // will want some sort of bounds checking here
  if( !mTrackedTouch )
  {
    mTrackedTouch = event.getTouches().front().getId();
    mAccumulatedDelta = 0;
  }
  mScrollMin = math<float>::min( mScrollMax, getHeight() * mScrollBottom - childHeight() );
  mScrollDirection = Vec2f::yAxis();
  mScrollDirection.rotate( getLocus()->getAccumulatedRotation() );
  mTimeline->removeTarget( &mScrollPosition );
}

void ScrollPane::touchesMoved(ci::app::TouchEvent &event)
{
  if( mightScroll() )
  {
    float delta = 0;
    for( auto touch : event.getTouches() )
    {
      if( touch.getId() == mTrackedTouch )
      {
        Vec2f d = touch.getPos() - touch.getPrevPos();
        delta = d.dot( mScrollDirection );
        mAccumulatedDelta += delta;
        if( !mScrolling && math<float>::abs(mAccumulatedDelta) > mScrollThreshold )
        { // in future, we'll also need some bottom-up bubbling so a scrollpane inside a scrollpane can work
          deepCancelInteractions();
          blockChildren();
          mScrolling = true;
        }
      }
    }
    if( mScrolling || math<float>::abs(mAccumulatedDelta) > mMoveThreshold )
    {
      setScroll( mScrollPosition() + delta );
    }
  }
}

void ScrollPane::touchesEnded(ci::app::TouchEvent &event)
{
  if( mightScroll() )
  {
    for( auto touch : event.getTouches() )
    {
      if( touch.getId() == mTrackedTouch )
      {
        if( !mScrolling )
        { // ease back to original position
          setScrollSoft( mScrollPosition() - mAccumulatedDelta, 0.2f );
        }
        else
        {
          Vec2f d = touch.getPos() - touch.getPrevPos();
          float delta = d.dot( mScrollDirection );
          float distance = delta * 3;
          //        float ease_time = lmap<float>( math<float>::clamp( math<float>::abs(distance), 0, 36 ), 0, 36, 0.2f, 0.6f );
          setScrollSoft( mScrollPosition() + distance, 0.2f );
        }
        mScrolling = false;
        mTrackedTouch = 0;
        unblockChildren();
      }
    }
  }
}

void ScrollPane::update()
{
  mTimeline->stepTo( mTimer.getSeconds() );
}

void ScrollPane::updateScroll()
{
  mScrollParent->setLoc( Vec2f( 0, mScrollPosition ) );
}

void ScrollPane::setScroll(float pos)
{
  mScrollPosition() = math<float>::clamp( pos, mScrollMin, mScrollMax );
  updateScroll();
}

void ScrollPane::setScrollSoft(float pos, float duration)
{
  timeline().apply( &mScrollPosition, math<float>::clamp( pos, mScrollMin, mScrollMax ), duration, EaseOutQuint() )
  .updateFn( [this](){ updateScroll(); } );
}

void ScrollPane::preChildDraw()
{ // TODO: set up stencil buffer or clipping planes
}

void ScrollPane::postChildDraw()
{ // unset clipping
}



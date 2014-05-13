//
//  ScrollPane.h
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "cinder/Timeline.h"
#include "cinder/Timer.h"
#include "Node.h"
#include "cinder/gl/Fbo.h"

namespace pockets
{
  /*
  Draws children into an FBO with scrolling.
  */

  typedef std::shared_ptr<class ScrollPane> ScrollPaneRef;
	class ScrollPane : public Node
	{
	public:
		ScrollPane( const ci::Vec2i &size );
		~ScrollPane();
    void connect( ci::app::WindowRef window );
    void touchesBegan( ci::app::TouchEvent &event );
    void touchesMoved( ci::app::TouchEvent &event );
    void touchesEnded( ci::app::TouchEvent &event );
    //! to get smoothed scroll ends, call update
    void update();
    //! set up stencil buffer
    void preChildDraw();
    //! unset stencil buffer
    void postChildDraw();
    void childAdded( NodeRef element );
    void setBottomScrollPercent( float bottom ){ mScrollBottom = bottom; }
    //! sets the move and scroll thresholds for responses (defaults to 4 and 10)
    void setThresholds( float move, float scroll );
    bool mightScroll() const { return mScrollMax - mScrollMin > mScrollThreshold; }
	private:
    ci::Anim<float>   mScrollPosition = 0;
    float             mScrollMin = 0;
    float             mScrollMax = 0;
    ci::Area          mOldViewport;
    ci::Area          mViewport;
    ci::Area          mDrawBounds;
    std::shared_ptr<ci::gl::SaveFramebufferBinding> mBindingSaver;
    // interaction bits
    ci::Vec2f         mScrollDirection = ci::Vec2f::yAxis();
    pk::Locus2dRef    mScrollParent = pk::Locus2dRef( new pk::Locus2d );
    uint32_t          mTrackedTouch = 0;
    bool              mScrolling = false;
    float             mAccumulatedDelta = 0.0f;
    float             mScrollThreshold = 10.0f;
    float             mMoveThreshold = 4.0f;
    ci::Timer         mTimer;
    ci::TimelineRef   mTimeline = ci::Timeline::create();
    // how high up can the bottom scroll (1.0 only to the bottom edge, 0.0 all the way to the top)
    float             mScrollBottom = 1.0f;

    void              updateScroll();
    void              setScroll( float pos );
    void              setScrollSoft( float pos, float duration );
    ci::Timeline&     timeline() const { return *mTimeline; }
	};
}

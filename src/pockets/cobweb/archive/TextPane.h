//
//  TextPane.h
//  WordShift
//
//  Created by David Wicks on 3/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/gl/Texture.h"
#include "cinder/Timeline.h"
#include "Node.h"

namespace pockets
{
  /*
  Holds a panel of text
  Animates to new text display when changed
  */
  typedef std::shared_ptr<class TextPane> TextPaneRef;
	class TextPane : public Node
	{
	public:
		TextPane( const ci::Font &font, int width );
    void update(float deltaTime);
    void draw();
    void setText( const std::string &text );
    void setTextSoft( const std::string &text, float delay );
    void setCentered( bool centered=true ){ mCentered = centered; }
	private:
    ci::Surface         mSurface;
    ci::gl::Texture     mTexture;
    ci::Rectf           mScreenBounds;  // in points
    ci::Area            mTextureRegion; // in pixels
    ci::Anim<ci::Vec2f> mTextureOffset = ci::Vec2f::zero(); // in pixels
    bool                mCentered = false;
    ci::TimelineRef     mTimeline = ci::Timeline::create();
	};
}

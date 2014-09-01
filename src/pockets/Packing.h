/*
 * Copyright (c) 2013 David Wicks, sansumbrella.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "Pockets.h"
#include "cinder/Rect.h"

namespace pockets
{
	/**
	 LocatedRectf:

	 A Rectf that can be placed at a location independent of its .
	 */
	/*
	class LocatedRectf : public ci::Rectf
	{
	public:
		LocatedRectf( const ci::vec2 &loc = ci::vec2( 0 ) ):
		mLoc( loc )
		{}
		ci::Rectf   getPlacedBounds() const { return *this + getLoc(); }
		ci::ivec2   getLoc() const { return mLoc; }
		void        setLoc( const ci::ivec2 &loc ){ mLoc = loc; }
	private:
		ci::vec2	mLoc;
	};
	*/

	/**
	 ScanlinePacker:

	 An online rectangle packer.
	 Pushing a rectangle into the collection adds it.
	 Need some kind of id system before allowing removal of objects.
	 */
	class ScanlinePacker
	{
	public:
		enum{
			eUnconstrained = -1
		};
		ScanlinePacker() {}
		ScanlinePacker( float maxWidth, float maxHeight = eUnconstrained ):
		mConstraints( maxWidth, maxHeight ),
		mPadding( 8.0f, 8.0f )
		{}
		//! adds a new \a rect to the packing and returns its index
		size_t		pushRect( ci::Rectf rect );
		//! returns the packed rectangle at \a index
		ci::Rectf	getRect ( size_t index ) const { return mRectangles.at( index ); }
		//! set the padding between rectangles
		void		setPadding( float gap ) { mPadding = ci::vec2( gap, gap ); }
		void		clear() { mRectangles.clear(); }
		//! iterators for traversal through rectangles
		std::vector<ci::Rectf>::const_iterator begin() const { return mRectangles.begin(); }
		std::vector<ci::Rectf>::const_iterator end() const { return mRectangles.end(); }
	private:
		std::vector<ci::Rectf>	mRectangles;
		size_t					mLastPackedRect;
		ci::vec2				mConstraints;
		ci::vec2				mPadding;
	};
	//! from a list of rectangles, get a list of located rectangles such that
	//! all fit in a space \a containerWidth wide
	std::vector<ci::Rectf> placeRects( const std::vector<ci::Rectf> &rectangles, float containerWidth );
}

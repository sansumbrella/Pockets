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

#include "Packing.h"

using namespace std;
using namespace cinder;
using namespace pockets;

size_t ScanlinePacker::pushRect( Rectf rect )
{	// TODO: better error handling
	assert( rect.getWidth() < mConstraints.x );
	if( rect.getUpperLeft() != vec2( 0 ) )
	{
		rect -= rect.getUpperLeft();
	}

	vec2 loc( mPadding );
	bool placed = false;
	while( !placed )
	{
		for( const Rectf &r : mRectangles )
		{
			Rectf bounds = r.inflated( mPadding );
			if( bounds.contains( loc ) )
			{	// move to the outside edge of bounds
				loc.x = bounds.getX2();
			}
		}
		if( loc.x + rect.getWidth() < mConstraints.x - mPadding.x )
		{	// probably placed, let's check our boundaries
			placed = true;
			Rectf potentialBounds = (rect + loc).inflated( vec2( -1.0f, -1.0f ) );
			for( const Rectf &r : mRectangles )
			{
				Rectf bounds = r.inflated( mPadding );
				if( potentialBounds.intersects( bounds ) )
				{
					placed = false;
				}
			}
		}
		if( !placed )
		{	// move to next row for continued evaluation
			loc.x = mPadding.x;
			loc.y += mPadding.y;	// could vary this step to optimize for grid
		}
		else
		{
			rect += loc;
			mRectangles.push_back( rect );
		}
	}
	return mRectangles.size() - 1;
}




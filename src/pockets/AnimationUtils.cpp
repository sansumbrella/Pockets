//
//  AnimationUtils.cpp
//  Energy
//
//  Created by David Wicks on 1/23/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "AnimationUtils.h"
#include <cmath>

using namespace ci;
using namespace pockets;
using namespace std;

Quatf pk::lerpQuaternion( const ci::Quatf &start, const ci::Quatf &end, float time )
{
  auto val = start.slerp( time, end ).normalized();
	return isfinite( val.getAxis() ) ? val : Quatf::identity();
}

bool pk::isfinite( const ci::Vec3f &vec ){ return std::isfinite( vec.x ) && std::isfinite( vec.y ) && std::isfinite( vec.z ); }

float pk::lerpShortestNormalized(float start, float end, float time)
{
	WrappedLerper lerpers[] = { WrappedLerper( start, end )
    , WrappedLerper( start + 1.0f, end )
    , WrappedLerper( start - 1.0f, end ) };
	auto shortest = std::min_element( lerpers, lerpers + 3
                                   , [](const WrappedLerper &lhs, const WrappedLerper &rhs)
                                   { return lhs.length() < rhs.length(); } );
	return shortest->getPosition( time );
}

ColorA pk::lerpHSVA(const ci::ColorA &start, const ci::ColorA &finish, float time)
{
	if( time == 0.0f ){ return start; }
	else if( time == 1.0f ){ return finish; }
	auto s_hsv = Color( start.r, start.g, start.b ).get( CM_HSV );
	auto f_hsv = Color( finish.r, finish.g, finish.b ).get( CM_HSV );
	return ColorA( CM_HSV
                , lerpShortestNormalized( s_hsv.x, f_hsv.x, time )
                , lerp( s_hsv.y, f_hsv.y, time )
                , lerp( s_hsv.z, f_hsv.z, time )
                , lerp( start.a, finish.a, time ) );
}

WrappedLerper::WrappedLerper( float start, float end ):
mStart( start ),
mEnd( end )
{}

float WrappedLerper::getPosition( float t )
{
	float pos = lerp( mStart, mEnd, t );
	if( pos < 0.0f ){ pos = 1.0f - pos; }
	pos -= math<float>::floor( pos );
	return pos;
}

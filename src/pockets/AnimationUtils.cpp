/*
 * Copyright (c) 2013, David Wicks
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

#include "AnimationUtils.h"
#include <cmath>

using namespace ci;
using namespace pockets;
using namespace std;

float pk::quantize( float f, float steps )
{
	return math<float>::floor(f * steps + 0.5f) / steps;
}

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

//function<float (float, float, float)> pk::wrappedLerpFn( float min_val, float max_val )
//{
//  return [=](float start, float end, float time)
//  {
//
//    return 0.0f;
//  };
//}

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

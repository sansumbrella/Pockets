/*
 * Copyright (c) 2014 David Wicks, sansumbrella.com
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

#include "ChoreographSample.h"
#include "cinder/Utilities.h"
#include "cinder/Easing.h"

using namespace std;
using namespace cinder;
using namespace choreograph;

ChoreographSample::ChoreographSample()
{}

ChoreographSample::~ChoreographSample()
{}

void ChoreographSample::setup()
{
  auto &sequence = _anim.move( &_ball_y )
    .startFn( [] (Motion<float> &c) { cout << "Start red" << endl; } )
    .getSequence().set( 5.0f ).hold( 0.5f ).rampTo( 500.0f, 1.0f, EaseInOutQuad() ).hold( 500.0f, 0.33f ).rampTo( 700.0f, 1.0f ).hold( 20.0f, 1.0f ).set( 400.0f );

  _anim.move( &_ball_2 )
    .startFn( [] (Motion<vec2> &c) { cout << "Start blue" << endl; } )
    .finishFn( [] (Motion<vec2> &c) { c.playbackSpeed( c.getPlaybackSpeed() * -1.0f ); } )
    .continuous( true )
    .updateFn( [&] (const vec2 &v) {
      vec2 size = app::getWindowSize();
      float shortest = std::min( v.x, size.x - v.x );
      shortest = std::min( shortest, size.y - v.y );
      shortest = std::min( shortest, v.y );
      _ball_radius = shortest;
    } )
    .getSequence().rampTo( vec2( app::getWindowSize() ) / 2.0f, 2.0f ).rampTo( vec2( app::getWindowSize() ), 2.0f ).rampTo( vec2( app::getWindowWidth() / 2.0f, 10.0f ), 3.0f ).rampTo( vec2( app::getWindowSize() ) / 2.0f, 0.5f );

/*
  _anim.move( &_ball_2 )
    .getSequence().set( Vec2f( 100.0f, 100.0f ) ).rampTo( Vec2f( 500.0f, 600.0f ), 6.0f, EaseOutBack() );
//*/

  for( float t = -1.0f; t < sequence.getDuration() + 0.2f; t += 0.1f )
  {
    app::console() << "Animation time: " << t << "\t\t, value: " << sequence.getValue( t ) << endl;
  }

  // example of reading time from string
  // for conversion operators, see http://en.cppreference.com/w/cpp/io/manip/get_time
  std::tm tm;
  std::stringstream ss("Jan 9 2014 12:35:34");
  ss >> std::get_time(&tm, "%b %d %Y %H:%M:%S");
  auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

void ChoreographSample::update(double dt)
{
  _anim.step( dt );
}

void ChoreographSample::draw()
{
  gl::disableDepthRead();
  gl::disableDepthWrite();

  gl::clear( Color( 0, 0, 0 ) );
  gl::color( Color( 1.0f, 0.0f, 0.0f ) );
  gl::drawSolidCircle( vec2( 200.0f, _ball_y ), 120.0f );
  gl::color( 0.0f, 0.0f, 1.0f );
  gl::drawSolidCircle( _ball_2, _ball_radius );

  gl::enableAlphaBlending( true );
  gl::color( Color::white() );
}


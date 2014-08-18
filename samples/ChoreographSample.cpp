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

using namespace std;
using namespace cinder;
using namespace choreograph;

ChoreographSample::ChoreographSample()
{}

ChoreographSample::~ChoreographSample()
{}

void ChoreographSample::setup()
{
  auto &rotation = _anim.add( &_ball_y ).hold( 5.0f, 1.0f ).rampTo( 500.0f, 3.0f ).hold( 500.0f, 1.0f ).rampTo( 20.0f, 2.0f ).hold( 20.0f, 1.0f ).hold( 400.0f, 1.0f );
  _anim.add( &_ball_2 ).hold( app::getWindowSize() / 2.0f, 2.0f ).rampTo( app::getWindowSize(), 2.0f ).rampTo( Vec2f( app::getWindowWidth() / 2.0f, 10.0f ), 3.0f ).rampTo( app::getWindowSize() / 2.0f, 0.5f );

  for( float t = -1.0f; t < rotation.getDuration(); t += 0.1f )
  {
    app::console() << "Animation time: " << t << "\t\t, value: " << rotation.getValue( t ) << endl;
  }
}

void ChoreographSample::update(double dt)
{
  _anim.step( dt );
}

void ChoreographSample::draw()
{
  gl::clear( Color( 0, 0, 0 ) );
  gl::color( Color( 1.0f, 0.0f, 0.0f ) );
  gl::drawSolidCircle( Vec2f( 200.0f, _ball_y ), 120.0f );
  gl::color( 0.0f, 0.0f, 1.0f );
  gl::drawSolidCircle( _ball_2, 60.0f );
}


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

  auto &rotation = _anim.move( &_ball_y )
    .startFn( [] (Motion<float> &c) { cout << "Start red" << endl; } )
    .getSequence().set( 5.0f ).hold( 1.0f ).rampTo( 500.0f, 3.0f, EaseInOutQuad() ).hold( 500.0f, 1.0f ).rampTo( 700.0f, 3.0f, CubicBezierEase( Vec2f( 0.25f, 0.5f ), Vec2f( 0.75f, 0.5f ) ) ).hold( 20.0f, 1.0f ).hold( 400.0f, 1.0f );

  _anim.move( &_ball_2 )
    .startFn( [] (Motion<Vec2f> &c) { cout << "Start blue" << endl; } )
    .finishFn( [] (Motion<Vec2f> &c) { c.speed( c.getSpeed() * -1.0f ); }  )
    .continuous( true )
    .updateFn( [&] (const Vec2f &v) {
      Vec2f size = app::getWindowSize();
      float shortest = min( v.x, size.x - v.x );
      shortest = min( shortest, size.y - v.y );
      shortest = min( shortest, v.y );
      _ball_radius = shortest;
    } )
    .getSequence().rampTo( app::getWindowSize() / 2.0f, 2.0f ).rampTo( app::getWindowSize(), 2.0f ).rampTo( Vec2f( app::getWindowWidth() / 2.0f, 10.0f ), 3.0f ).rampTo( app::getWindowSize() / 2.0f, 0.5f );

  for( float t = -1.0f; t < rotation.getDuration() + 0.2f; t += 0.1f )
  {
    app::console() << "Animation time: " << t << "\t\t, value: " << rotation.getValue( t ) << endl;
  }

  Font font( "Monaco", 24.0f );
  string urdu = loadString( loadFile( "/Users/davidwicks/Client/Active/soso/urdu-utf8.txt" ) );
  string chinese = loadString( loadFile( "/Users/davidwicks/Client/Active/soso/chinese-utf8.txt" ) );
  string kana = loadString( loadFile( "/Users/davidwicks/Client/Active/soso/kana-utf8.txt" ) );
  TextLayout layout;
  layout.clear( Color::black() );
  layout.setColor( Color::white() );
  layout.setFont( font );
  layout.addLine( urdu );
  layout.addLine( chinese );
  layout.addLine( kana );
  _text = gl::Texture::create( layout.render( true, true ) );
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
  gl::drawSolidCircle( Vec2f( 200.0f, _ball_y ), 120.0f );
  gl::color( 0.0f, 0.0f, 1.0f );
  gl::drawSolidCircle( _ball_2, _ball_radius );

  gl::enableAlphaBlending( true );
  gl::color( Color::white() );
  gl::draw( _text );
}


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

#pragma once

#include "pockets/Scene.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"

namespace choreograph
{

// A motion describes a one-dimensional change through time.
// These can be ease equations, always return the same value, or sample some data. Whatever you want.
// For classic motion, Motion( 0 ) = 0, Motion( 1 ) = 1.
// set( 0.0f ).move( )
// then again, might just make a curve struct that is callable and have a way to set its initial and final derivatives.
typedef std::function<float (float)> Motion;

struct Hold
{
  float operator() ( float t ) const { return 0.0f; }
};

struct LinearRamp
{
  float operator() ( float t ) const { return t; }
};

// A position describes a point in time.

template<typename T>
struct Position
{
  T     value;
  float time;
};

template<typename T>
T lerpT( const T &a, const T &b, float t )
{
  return a + (b - a) * t;
}

template<typename T>
struct Segment
{
  Position<T> start;
  Position<T> end;
  Motion      motion;
  std::function<T (const T&, const T&, float)> lerpFn = &lerpT<T>;

  T getValue( float atTime ) const
  {
    float t = (atTime - start.time) / (end.time - start.time);
    return lerpFn( start.value, end.value, motion( t ) );
  }
};

/*
 Sequence of Positions on a given type, interpolated with Motions.
 Give an output pointer to send the value to.
*/
template<typename T>
class Sequence
{
public:
  T getValue( float atTime );

  Sequence<T>& set( const T &value )
  {
    if( _segments.empty() ) {
      _initial_value = value;
    }
    else {
      hold( 0.0f );
    }

    return *this;
  }

  Sequence<T>& hold( float duration )
  {
    return hold( endValue(), duration );
  }

  Sequence<T>& hold( const T &value, float duration )
  {
    Segment<T> s;
    s.start = Position<T>{ value, _duration };
    s.end = Position<T>{ value, _duration + duration };
    s.motion = Hold();

    _segments.push_back( s );

    _duration += duration;

    return *this;
  }

  Sequence<T>& rampTo( const T &value, float duration )
  {
    Segment<T> s;
    s.start = Position<T>{ endValue(), _duration };
    s.end = Position<T>{ value, _duration + duration };
    s.motion = LinearRamp();

    _segments.push_back( s );

    _duration += duration;

    return *this;
  }

  float getDuration() const { return _duration; }

private:
  std::vector<Segment<T>>  _segments;
  T     _initial_value;
  T     endValue() const { return _segments.empty() ? _initial_value : _segments.back().end.value; }
  Segment<T>& endSegment() const { return _segments.back(); }
  float _duration = 0.0f;

  friend class Animation;
};

template<typename T>
T Sequence<T>::getValue( float atTime )
{
  if( atTime < 0.0f )
  {
    return _initial_value;
  }

  auto iter = _segments.begin();
  while( iter < _segments.end() ) {
    if( (*iter).end.time > atTime )
    {
      return (*iter).getValue( atTime );
    }
    ++iter;
  }
  // past the end, get the final value
  return endValue();
}

// Non-templated base type so we can store in a polymorphic container.
class Connect
{
public:
  virtual ~Connect() = default;
  virtual void step( float dt ) = 0;
};

// Drives a Sequence and sends its value to a user-defined variable.
// Might mirror the Sequence interface for easier animation.
template<typename T>
class Connection : public Connect
{
public:
  void step( float dt ) override
  {
    last_time = time;
    time += dt * _speed;

    if( _startFn ) {
      if( _speed > 0.0f && time > 0.0f && last_time <= 0.0f )
        _startFn( *this );
      else if( _speed < 0.0f && time < sequence->getDuration() && last_time >= sequence->getDuration() )
        _startFn( *this );
    }

    *output = sequence->getValue( time );
    if( _updateFn ) {
      _updateFn( *output );
    }

    if( _finishFn ){
      if( _speed > 0.0f && time >= sequence->getDuration() && last_time < sequence->getDuration() )
        _finishFn( *this );
      else if( _speed < 0.0f && time <= 0.0f && last_time > 0.0f )
        _finishFn( *this );
    }
  }

  //! Returns the underlying sequence for extension.
  Sequence<T>&  getSequence() { return *sequence; }

  typedef std::function<void (const T&)>        DataCallback;
  typedef std::function<void (Connection<T> &)> Callback;
  //! Set a function to be called when we reach the end of the sequence.
  Connection<T>& finishFn( const Callback &c ) { _finishFn = c; return *this; }
  //! Set a function to be called when we start the sequence.
  Connection<T>& startFn( const Callback &c ) { _startFn = c; return *this; }
  //! Set a function to be called at each update step of the sequence. Called immediately after setting the target value.
  Connection<T>& updateFn( const DataCallback &c ) { _updateFn = c; return *this; }

  float          getSpeed() const { return _speed; }
  Connection<T>& speed( float s ) { _speed = s; return *this; }

  // shared_ptr to sequence since many connections could share the same sequence
  // this enables us to to pseudo-instancing on our animations, reducing their memory footprint.
  std::shared_ptr<Sequence<T>> sequence;
  T           *output;
  Callback    _finishFn = nullptr;
  Callback    _startFn = nullptr;
  DataCallback _updateFn = nullptr;

  // Playback speed. Set to negative to go in reverse.
  float       _speed = 1.0f;
  // Current animation time in seconds.
  float       time = 0.0f;
  // Previous animation time in seconds.
  float       last_time = 0.0f;
};

/*
 Holds a collection of connected sequences.
 Maybe variadic templates to specify an animation with different channel types,
 or one composed of n existing channels...
 */

class Animation
{
public:

  //! Create a Sequence that is connected out to \a output.
  template<typename T>
  Connection<T>& drive( T *output )
  {
    auto c = std::make_shared<Connection<T>>();
    c->sequence = std::make_shared<Sequence<T>>();
    c->output = output;
    c->sequence->_initial_value = *output;
    _connections.push_back( c );
    return *c;
  }

  //! Create a Connection that plays \a sequence into \a output.
  template<typename T>
  Connection<T>& drive( T *output, std::shared_ptr<Sequence<T>> sequence )
  {
    auto c = std::make_shared<Connection<T>>();
    c->sequence = sequence;
    c->output = output;
    c->sequence->_initial_value = *output;
    _connections.push_back( c );
    return *c;
  }

  // Thinking about this one
  // Create a Sequence with an output slot for type T.
  template<typename T>
  Sequence<T>& move( uint32_t label )
  {

  }

  // Assign the output slot of Sequence at \a label to \a output.
  template<typename T>
  void connect( uint32_t label, T *output )
  {

  }

  // Advance all current connections.
  void step( float dt )
  {
    for( auto &c : _connections )
    {
      c->step( dt );
    }
  }

private:
  std::vector<std::shared_ptr<Connect>> _connections;
};

}

namespace co = choreograph;

class ChoreographSample : public pk::Scene
{
public:
  ChoreographSample();
  ~ChoreographSample();

  void setup() override;
  void update( double dt ) override;
  void draw() override;

private:
  float                 _ball_y;
  ci::Vec2f             _ball_2;
  float                 _ball_radius;
  co::Animation         _anim;
  ci::gl::TextureRef    _text;

};

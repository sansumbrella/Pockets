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

namespace choreograph
{

// A motion describes a one-dimensional change through time.
// These can be ease equations, always return the same value, or sample some data. Whatever you want.
// For classic motion, Motion( 0 ) = 0, Motion( 1 ) = 1.
// set( 0.0f ).move( )
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

  Sequence<T>& hold( const T &value, float duration )
  {
    Segment<T> s;
    s.start = Position<T>{ value, _end_time };
    s.end = Position<T>{ value, _end_time + duration };
    s.motion = Hold();

    _segments.push_back( s );

    _end_value = value;
    _end_time += duration;

    return *this;
  }

  Sequence<T>& rampTo( const T &value, float duration )
  {
    Segment<T> s;
    s.start = Position<T>{ _end_value, _end_time };
    s.end = Position<T>{ value, _end_time + duration };
    s.motion = LinearRamp();

    _segments.push_back( s );

    _end_value = value;
    _end_time += duration;

    return *this;
  }

  float getDuration() const { return _end_time; }

private:
  std::vector<Segment<T>>  _segments;
  T     _end_value;
  float _end_time = 0.0f;
};

template<typename T>
T Sequence<T>::getValue( float atTime )
{
  auto iter = _segments.begin();
  while( iter < _segments.end() ) {
    if( (*iter).end.time > atTime )
    {
      return (*iter).getValue( atTime );
    }
    ++iter;
  }
  // past the end, get the final value
  return _end_value;
}

// Non-templated base type so we can store in a polymorphic container.
class Connect
{
public:
  virtual ~Connect() = default;
  virtual void step( float dt ) = 0;
};

// Pipes the value from a Sequence out to a user-defined variable.
template<typename T>
class Connection : public Connect
{
public:
  void step( float dt ) override
  {
    time += dt;
    *output = sequence->getValue( time );
  }
  std::shared_ptr<Sequence<T>> sequence;
  T           *output;
  float       time = 0.0f;
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
  Sequence<T>& sequence( T *output )
  {
    auto c = std::make_shared<Connection<T>>();
    c->sequence = std::make_shared<Sequence<T>>();
    c->output = output;
    _connections.push_back( c );
    return *(c->sequence);
  }

  // Thinking about this one
  // Create a Sequence with an output slot for type T.
  template<typename T>
  Sequence<T>& sequence( uint32_t label )
  {

  }

  // Assign the output slot of Sequence at \a label to \a output.
  template<typename T>
  void connect( uint32_t label, T *output )
  {

  }

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
  co::Animation         _anim;
};

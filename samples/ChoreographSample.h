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

/*
 Act on a type.
 */
template<typename T>
class Motion
{
public:
  virtual ~Motion() = default;
  virtual T getValue( float atTime ) = 0;
  inline float normalizedTime( float t ) const
  {
    return (end == start) ? 1.0f : (t - start) / (end - start);
  }

  float start = 0.0f;
  float end = 0.0f;
};

template<typename T>
class Ramp : public Motion<T>
{
public:
  T getValue( float atTime ) override
  {
    return start_value + (end_value - start_value) * Motion<T>::normalizedTime( atTime );
  }
  T start_value;
  T end_value;
  // ease function, etc.
};

template<typename T>
class Hold : public Motion<T>
{
public:
  T getValue( float atTime ) override { return value; }
  T value;
};

/*
 Sequence of motions on a given type.
 Give an output pointer to send the value to.
*/
template<typename T>
class Sequence
{
public:
  T getValue( float atTime );

  Sequence<T>& hold( const T &value, float duration )
  {
    auto hold = std::make_shared<Hold<T>>();
    hold->value = value;
    hold->start = _end_time;
    hold->end = _end_time = _end_time + duration;
    _end_value = value;
    _events.push_back( hold );

    return *this;
  }

  Sequence<T>& rampTo( const T &value, float duration )
  {
    auto ramp = std::make_shared<Ramp<T>>();
    ramp->start_value = _end_value;
    ramp->end_value = value;
    ramp->start = _end_time;
    ramp->end = _end_time = _end_time + duration;
    _end_value = value;

    _events.push_back( ramp );

    return *this;
  }

  float getDuration() const { return _end_time; }

private:
  std::vector<std::shared_ptr<Motion<T>>> _events;
  T     _end_value;
  float _end_time = 0.0f;
};

template<typename T>
T Sequence<T>::getValue( float atTime )
{
  auto iter = _events.begin();
  while( iter < _events.end() ) {
    if( (*iter)->end > atTime )
    {
      return (*iter)->getValue( atTime );
    }
    ++iter;
  }
  // past the end, get the final value
  return _events.back()->getValue( _events.back()->end );
}

class Connect
{
public:
  virtual ~Connect() = default;
  virtual void step( float dt ) = 0;
};

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

  template<typename T>
  Sequence<T>& add( T *output )
  {
    auto c = std::make_shared<Connection<T>>();
    c->sequence = std::make_shared<Sequence<T>>();
    c->output = output;
    _connections.push_back( c );
    return *(c->sequence);
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

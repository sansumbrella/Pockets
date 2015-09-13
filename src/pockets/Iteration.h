/*
 * Copyright (c) 2015 David Wicks, sansumbrella.com
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
#include <utility>
#include <iterator>

namespace pockets
{

template <typename Collection>
class ReverseViewT
{
public:
  ReverseViewT(Collection &collection)
  : _collection(collection)
  {}

  auto begin() { return _collection.rbegin(); }
  auto end() { return _collection.rend(); }
  auto begin() const { return _collection.rbegin(); }
  auto end() const { return _collection.rend(); }

private:
  Collection &_collection;
};

template <typename Collection>
ReverseViewT<Collection> reverse_view(Collection &c)
{
  return ReverseViewT<Collection>(c);
}

template <typename Collection>
class ViewT
{
public:
  using Iterator = decltype(Collection().begin());

  ViewT(Iterator begin, Iterator end)
  : _begin(begin),
    _end(end)
  {}

  auto begin() const { return _begin; }
  auto end() const { return _end; }

private:
  Iterator _begin;
  Iterator _end;
};

///
/// Returns a view for iterating over part of a collection.
///
template <typename Collection>
ViewT<Collection> partial_view(Collection &c, size_t begin, size_t end)
{
  return ViewT<Collection>(std::next(c.begin(), begin), std::next(c.begin(), end));
}

///
/// A numeric range.
///
template <typename Number>
class RangeT
{
public:
  explicit RangeT(Number end)
  : _end(end)
  {}

  RangeT(Number begin, Number end, Number step)
  : _begin(begin),
    _end(end),
    _step(step)
  {}

  struct Iterator
  {
    Iterator(Number value, Number step, Number end)
    : _value(value),
      _step(step),
      _end(end)
    {}

    /// Dereferencing returns the iterator's value.
    Number operator *() { return _value; }
    const Iterator& operator ++()
    {
      _value += _step;
      return *this;
    }

    /// Comparator for range-based for loops. Not generally useful.
    bool operator !=(const Iterator &rhs)
    {
      return _step > 0 ? _value < rhs._end : _value > rhs._end;
    }
  private:
    Number _value;
    Number _step;
    Number _end;
  };

  auto begin() const { return Iterator{_begin, _step, _end}; }
  auto end() const { return Iterator{_end, _step, _end}; }

private:
  Number _begin = 0;
  Number _end = 0;
  Number _step = 1;
};

template <typename Number>
RangeT<Number> range(Number begin, Number end, Number step=1)
{
  return RangeT<Number>(begin, end, step);
}

template <typename Collection>
class EnumeratingViewT
{
public:
  EnumeratingViewT(Collection &collection)
  : _collection(collection)
  {}

  using ItemT = decltype(*Collection().begin());
  using CollectionIterator = decltype(Collection().begin());

  /// A Point in the collection.
  struct Point
  {
    size_t index;
    ItemT& value;
  };

  struct Iterator
  {
  public:
    Iterator(size_t index, CollectionIterator iterator)
    : _index(index),
      _iterator(iterator)
    {}

    const Iterator& operator ++()
    {
      _index += 1;
      ++_iterator;
      return *this;
    }

    Point operator *() const
    {
      return Point{_index, *_iterator};
    }

    bool operator != (const Iterator &rhs)
    {
      return _iterator != rhs._iterator;
    }
  private:
    size_t              _index = 0;
    CollectionIterator  _iterator;
  };

  auto begin() { return Iterator(0, _collection.begin()); }
  auto end() { return Iterator(_collection.size(), _collection.end()); }
private:
  Collection &_collection;
};

///
/// Generates a range expression such that a counting number is provided with each element in the original collection.
/// for(auto p: enumerate(vector)) {
///   p.value == vector[p.index];
/// }
///
template <typename Collection>
EnumeratingViewT<Collection> enumerate(Collection &c)
{
  return EnumeratingViewT<Collection>(c);
}

}

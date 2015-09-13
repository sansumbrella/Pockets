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

template <typename Collection>
class EnumeratorT
{
public:
  EnumeratorT(Collection &collection)
  : _collection(collection)
  {}

  using ItemT = decltype(Collection().front());
  using CollectionIterator = decltype(Collection().begin());

  struct Object
  {
  public:
    size_t index() const { return _index; }
    ItemT& value() const { return *_iterator; }
  private:
    size_t              _index = 0;
    CollectionIterator  _iterator;
  };

  auto begin() { return std::make_pair(0, _collection.begin()); }
  auto end() { return std::make_pair(_collection.size() - 1, _collection.end()); }
private:
  Collection &_collection;
};

template <typename Collection>
EnumeratorT<Collection> enumerate(Collection &c)
{
  return EnumeratorT<Collection>(c);
}

}

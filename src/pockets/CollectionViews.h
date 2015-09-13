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

#pragma once
#include "Pockets.h"
#include "CollectionViews.inl"

namespace pockets
{
  ///
  /// Generates a range expression that traverses a collection in reverse.
  /// for (auto &value: reverse_view(c));
  ///
  template <typename Collection>
  ReverseViewT<Collection> reverse_view(Collection &c)
  {
    return ReverseViewT<Collection>(c);
  }

  ///
  /// Returns a view for iterating over part of a collection.
  /// for (auto &value: partial_view(c, 0, 5));
  ///
  template <typename Collection>
  ViewT<Collection> partial_view(Collection &c, size_t begin, size_t end)
  {
    return ViewT<Collection>(std::next(c.begin(), begin), std::next(c.begin(), end));
  }

  ///
  /// Generates a range expression so you can get numbers from a range-based for loop.
  /// for (auto i: range(0, 1000));
  ///
  template <typename Number>
  RangeT<Number> range(Number begin, Number end, Number step=1)
  {
    return RangeT<Number>(begin, end, step);
  }

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

} // namespace pockets

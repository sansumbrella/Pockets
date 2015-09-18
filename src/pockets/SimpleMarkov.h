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

namespace pockets {

///
/// A Simple Markov Node.
/// Stores a shared_ptr to some meaningful node-y type.
/// Could store by value for better performance, but whatever for now.
///
template <typename T>
struct MarkovNode
{
  using ThingRef = std::shared_ptr<T>;
  using MarkovRef = std::shared_ptr<MarkovNode>;

  explicit MarkovNode(const ThingRef &thing)
  : thing(thing)
  {}

  struct Exit
  {
    MarkovRef  exit;
    float     weight = 1.0f;
  };

  void addExit(const MarkovRef &t, float weight)
  {
    exits.push_back({t, weight});
  }

  /// Return the exit at normalized position t.
  MarkovRef findExit(float t)
  {
    auto possibilities = 0.0f;
    for (auto &e: exits)
    {
      possibilities += e.weight;
    }
    auto value = t * possibilities;

    for (auto &e: exits)
    {
      value -= e.weight;
      if (value <= 0.0f)
      {
        return e.exit;
      }
    }
    return nullptr;
  }

  std::vector<Exit>    exits;
  std::shared_ptr<T>   thing;
};

///
/// Create a markov node.
/// Uses shared_ptr since we need to store the nodes inside of other nodes.
///
template <typename T>
std::shared_ptr<MarkovNode<T>> createNode(const std::shared_ptr<T> &thing)
{
  return std::make_shared<MarkovNode<T>>(thing);
}

} // namespace pockets

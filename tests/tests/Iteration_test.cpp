//
//  Iteration_test.cpp
//
//  Created by David Wicks on 9/13/15.
//  Copyright (c) 2015 David Wicks. All rights reserved.
//

#include "catch.hpp"
#include "pockets/CollectionViews.h"
#include <vector>
#include <unordered_set>
#include <iostream>

using namespace std;

TEST_CASE("Iteration_test")
{
  auto collection = vector<int>{ 1, 2, 3, 4, 5, 6 };
  auto unordered_collection = set<string>{ "hi", "hello", "tony" };

  SECTION("reverse_view allows us to walk a collection backwards.")
  {
    auto forward = vector<int>();
    auto reversed = vector<int>();

    for (auto &i: collection)
    {
      forward.push_back(i);
    }

    for (auto &i: pk::reverse_view(collection))
    {
      reversed.push_back(i);
    }

    REQUIRE(forward == collection);
    REQUIRE(reversed == (vector<int>{ 6, 5, 4, 3, 2, 1 }));
  }

  SECTION("partial_view allows us to look at a subset of a collection.")
  {
    auto first = vector<int>();
    auto last = vector<int>();
    auto copy = vector<int>();

    for (auto &i: pk::partial_view(collection, 0, collection.size() / 2))
    {
      first.push_back(i);
    }

    for (auto &i: pk::partial_view(collection, 0, collection.size()))
    {
      copy.push_back(i);
    }

    for (auto &i: pk::partial_view(collection, 3, collection.size()))
    {
      last.push_back(i);
    }

    REQUIRE(copy == collection);
    REQUIRE(first == (vector<int>{ 1, 2, 3 }));
    REQUIRE(last == (vector<int>{ 4, 5, 6 }));
  }

  SECTION("Ascending and descending numeric ranges can be used as generators.")
  {
    for (auto i: pk::range(0.0f, 10.0f, 1.0f))
    {
      cout << i << endl;
    }

    for (auto i: pk::range(20.0f, 0.0f, -1.0f))
    {
      cout << i << endl;
    }
  }

  SECTION("enumerate allows us to walk a collection with corresponding counting numbers.")
  {
    for (auto p: pk::enumerate(collection))
    {
      REQUIRE(p.value == collection.at(p.index));
    }

    // For non-sequential containers, the indices can still be used with std::next.
    for (auto p: pk::enumerate(unordered_collection))
    {
      REQUIRE(p.value == *std::next(unordered_collection.begin(), p.index));
    }
  }

  // Untested (and hence currently unsupported):
  // combining different views onto the same collection.
}

//
//  Markov_test.cpp
//
//  Created by David Wicks on 9/17/15.
//  Copyright © 2015 David Wicks. All rights reserved.
//

#include "catch.hpp"
#include "SimpleMarkov.h"
#include <iostream>

using namespace pockets;
using namespace std;

TEST_CASE("Markov_test")
{
  struct Thing
  {
    Thing() = default;
    explicit Thing(const std::string &name)
    : name(name)
    {}

    std::string name;
  };

	SECTION("Empty markov chains return no exit")
  {
    auto thing = make_shared<Thing>("Sartre");
    auto thing_node = createNode(thing);

    auto exit = thing_node->findExit(0);

    REQUIRE(exit == nullptr);
  }

  SECTION("Markov chains with one exit return that exit")
  {
    auto thing = createNode(make_shared<Thing>("Base"));
    thing->addExit(createNode(make_shared<Thing>("Exit One")), 1.0f);

    REQUIRE(thing->findExit(0.5f)->thing->name == "Exit One");
  }


  SECTION("Markov chains with weighted exits pick between exits based on weight.")
  {
    auto thing = createNode(make_shared<Thing>("Base"));
    thing->addExit(createNode(make_shared<Thing>("Exit One")), 1.0f);
    thing->addExit(createNode(make_shared<Thing>("Exit Two")), 9.0f);

    REQUIRE(thing->findExit(0.05f)->thing->name == "Exit One");
    REQUIRE(thing->findExit(0.15f)->thing->name == "Exit Two");
    REQUIRE(thing->findExit(0.9f)->thing->name == "Exit Two");
  }

  SECTION("Alternative markov structure")
  {
    MarkovGraph<string> string_graph;
    // Keep in mind that the map implementation means that the paths aren't
    // necessarily traversed in the order they are added.
    string_graph.addPathway("Hello", "Goodbye", 1.0f);
    string_graph.addPathway("Hello", "Hello", 0.5f);

    string_graph.addPathway("Goodbye", "Hello", 0.5f);
    string_graph.addPathway("Goodbye", "Goodbye", 0.5f);

    cout << string_graph.nextNode("Hello", 0.32f) << endl;
    cout << string_graph.nextNode("Hello", 1.0f) << endl;
    cout << string_graph.nextNode("Goodbye", 0.4f) << endl;
  }
}

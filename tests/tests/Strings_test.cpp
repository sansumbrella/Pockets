//
//  Strings_test.cpp
//
//  Created by Soso Limited on 10/1/15.
//  Copyright © 2015 David Wicks. All rights reserved.
//

#include "catch.hpp"
#include "StringUtilities.h"
#include <iostream>

using namespace pockets;
using namespace std;

TEST_CASE("Strings_test")
{

  SECTION("Text is trimmed only on the given sides")
  {
    const auto input = "  whatever  ";

    REQUIRE(trim_right(input) == "  whatever");
    REQUIRE(trim_left(input) == "whatever  ");
    REQUIRE(trim(input) == "whatever");
  }

  SECTION("Empty Strings Are Left Alone")
  {
    const auto empty = "";
    REQUIRE(trim(empty) == empty);
    REQUIRE(trim_right(empty) == empty);
    REQUIRE(trim_left(empty) == empty);
  }

  SECTION("Clean strings are unmodified by trimming")
  {
    const auto clean = "clean";

    REQUIRE(trim(clean) == clean);
    REQUIRE(trim_right(clean) == clean);
    REQUIRE(trim_left(clean) == clean);
  }

  SECTION("Whitespace-only strings are returned empty")
  {
    const auto spaces = "  \t ";
    const auto empty = "";

    REQUIRE(trim_right(spaces) == empty);
    REQUIRE(trim_left(spaces) == empty);
    REQUIRE(trim(spaces) == empty);
  }

  SECTION("Text with UTF-8 characters might break trim and trim_right")
  {
    const auto unicode_stuff = string(u8"\u0121\u0107\u0120\u0A20\u0C20");
    const auto left_space = string(u8" ");
    const auto right_space = string(u8" \t");
    const auto fancy = left_space + unicode_stuff + right_space;

    REQUIRE(trim_right(fancy) == (left_space + unicode_stuff));
    REQUIRE(trim_left(fancy) == (unicode_stuff + right_space));
    REQUIRE(trim(fancy) == unicode_stuff);

    cout << "G dot: " << fancy << "a" << endl;
    cout << "G dot: " << trim_right(fancy) << "a" << endl;
  }
}

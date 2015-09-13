//
//  Iteration_test.cpp
//
//  Created by David Wicks on 9/13/15.
//  Copyright (c) 2015 David Wicks. All rights reserved.
//

#include "catch.hpp"

TEST_CASE("Iteration_test")
{
	auto a = 0;
	auto b = 1;

	SECTION("Write a failing test first to confirm your new test is run.")
	{
		REQUIRE(a == b);
	}
}

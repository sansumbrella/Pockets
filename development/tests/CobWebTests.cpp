#define CATCH_CONFIG_MAIN
#include "catch.hpp"

bool exists()
{
	return true;
}

TEST_CASE( "Function exists" ) {
	REQUIRE( exists() );
}

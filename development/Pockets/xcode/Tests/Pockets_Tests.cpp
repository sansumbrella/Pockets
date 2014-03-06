//
//  PupTent_Tests.cpp
//  PupTent Tests
//
//  Created by David Wicks on 3/4/14.
//
//

#define CATCH_CONFIG_MAIN
#include "3rdparty/catch.hpp"

#include "pockets/AnimationUtils.h"
using namespace pockets;
using namespace cinder;

TEST_CASE( "Circuler Lerp", "[animation]" ) {
	REQUIRE( lerpWrapped( 0.1f, 1.0f, 1.0f, 0.5f ) == 1.05f );
	REQUIRE( lerpWrapped( 0.6f, 1.0f, 1.0f, 0.5f ) == 0.8f );
	REQUIRE( lerpWrapped( 0.2f, 1.0f, 1.0f, 0.5f ) == 1.1f );
	REQUIRE( lerpWrapped( 0.5f, 1.0f, 1.0f, 0.5f ) == 0.75f );
}

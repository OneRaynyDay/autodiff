#include "catch.hpp"
#include "etree.h"

#define CATCH_CONFIG_MAIN // Make our main

TEST_CASE( "et::var can be initialized. (In different ways)", "[et::var]" ) {
    et::var x(10);
}

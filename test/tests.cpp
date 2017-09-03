// NOTE: Defining CATCH_CONFIG_MAIN must be
// done *before* including the header file.
// The header file checks to see if the macro was
// already created.

#define CATCH_CONFIG_MAIN // Make our main
#include "catch.hpp"
#include "../src/etree.h"


TEST_CASE( "et::var can be initialized. (In different ways)", "[et::var::var]" ) {
    et::var x(10);
    REQUIRE(x.value() == 10);

    SECTION( "et::var can be initialized via copying." ) {
        et::var y(x); 
        REQUIRE(y.value() == 10);
    }
}

TEST_CASE( "et::var can be added.", "[et::var::operator+]" ) {
    et::var x(10), y(20);
    
    SECTION( "Add 2 et::vars." ){
        et::var z = x + y;

        REQUIRE(z.value() == 30);
    }

    SECTION( "Left add 10 + x." ){
        et::var z = 10 + x;

        REQUIRE(z.value() == 20);
    }
    
    SECTION( "Right add y + 20." ){
        et::var z = y + 20;

        REQUIRE(z.value() == 40);
    }
}

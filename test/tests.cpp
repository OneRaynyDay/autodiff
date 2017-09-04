// NOTE: Defining CATCH_CONFIG_MAIN must be
// done *before* including the header file.
// The header file checks to see if the macro was
// already created.

#define CATCH_CONFIG_MAIN // Make our main
#include "catch.hpp"
#include "../src/etree.h"

#define NEW_CASE std::cout<<"======="<<std::endl;
#define NEW_SEC  std::cout<<"-------"<<std::endl;

TEST_CASE( "et::var can be initialized. (In different ways)", "[et::var::var]" ) {
    et::var x(10);
    REQUIRE(x.getValue() == 10);

    SECTION( "et::var can be initialized via copying." ) {
        et::var y(x);
        REQUIRE(y.getValue() == 10);
    }

    SECTION( "et::var can be initialized via operator=." ) {
        et::var y = 3;
        REQUIRE(y.getValue() == 3);
    }
}

// TODO: Support unit tests for EACH operator.
TEST_CASE( "et::var can be added.", "[et::var::operator+]" ) {
    et::var x(10), y(20);

    SECTION( "Add 2 et::vars." ){
        et::var a(8), b(2), c(12), d(8);
        et::var x = a + b;
        et::var y = c + d;
        et::var z = x + y;

        REQUIRE(z.getChildren().size() == 2);

        SECTION( "Require that the members are the same. (Not copied)"){
            REQUIRE(z.getChildren()[0]->getChildren()[0].get() ==
                    x.getChildren()[0].get());
            REQUIRE(z.getChildren()[0]->getChildren()[0]->getValue() == 8);
            REQUIRE(z.getChildren()[1]->getChildren()[1]->getValue() == 8);
        }
    }

    SECTION( "Left add 10 + x." ){
        et::var z = 15 + x;

        REQUIRE(z.getChildren().size() == 2);

        SECTION( "Require that the members are the same. (Not copied)"){
            REQUIRE(z.getChildren()[0].get()->getValue() == 15);
            REQUIRE(z.getChildren()[1].get()->getValue() == 10);
        }
    }

    SECTION( "Right add y + 20." ){
        et::var z = y + 15;

        REQUIRE(z.getChildren().size() == 2);

        SECTION( "Require that the members are the same. (Not copied)"){
            REQUIRE(z.getChildren()[0].get()->getValue() == 20);
            REQUIRE(z.getChildren()[1].get()->getValue() == 15);
        }
    }
}

TEST_CASE( "et::var can be exponentiated.", "[et::var::exp]" ) {
    et::var y(10);
    et::var x = y + 10;

    SECTION( "Exponentiate an et::var." ){
        et::var z = et::exp(x);
        REQUIRE(z.getChildren().size() == 1);

        SECTION( "Require that the members are the same. (Not copied)" ){
            REQUIRE(z.getChildren()[0]->getChildren()[0].get() ==
                    x.getChildren()[0].get());
            REQUIRE(z.getChildren()[0]->getChildren()[0]->getValue() == 10);
        }
    }
}

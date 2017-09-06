#include "catch.hpp"
#include "../src/var.h"

#define NEW_CASE std::cout<<"======="<<std::endl;
#define NEW_SEC  std::cout<<"-------"<<std::endl;

TEST_CASE( "et::var can be initialized. (In different ways)", "[et::var::var]" ) {
    et::var x(10);
    REQUIRE(x.getValue() == 10);

    SECTION( "et::var can be initialized via move copying." ) {
        et::var y(et::var(3));
        REQUIRE(y.getValue() == 3);
        REQUIRE(y.getUseCount() == 1);
    }

    SECTION( "et::var can be initialized via shallow copying." ){
        et::var y(x);
        REQUIRE(y.getValue() == 10);
        REQUIRE(y.getUseCount() == 2);
    }
    
    SECTION( "et::var can be initialized via deep copying." ){
        et::var y = x.clone();
        REQUIRE(y.getValue() == 10);
        REQUIRE(y.getUseCount() == 1);
    }

    SECTION( "et::var can be initialized via move operator=." ) {
        et::var y = 3;
        REQUIRE(y.getValue() == 3);
        REQUIRE(y.getUseCount() == 1);
    }
}

// // TODO: Support unit tests for EACH operator.
TEST_CASE( "et::var can be added.", "[et::var::operator+]" ) {
    et::var x(10), y(20);

    SECTION( "Add 2 et::vars." ){
        et::var a(8), b(2), c(12), d(8);
        et::var x = a + b;
        et::var y = c + d;
        et::var z = x + y;

        REQUIRE(z.getChildren().size() == 2);
        
        SECTION( "Require that the members are being added refcounts." ){
            REQUIRE(z.getChildren()[0].getUseCount() == 2);
            REQUIRE(z.getChildren()[0].getChildren()[0].getUseCount() == 2);
        }

        SECTION( "Require that the members are the same. (Not copied)" ){
            REQUIRE(z.getChildren()[0].getChildren()[0].getValue() == 8);
            REQUIRE(z.getChildren()[1].getChildren()[1].getValue() == 8);
        }
    }

    SECTION( "Left add 10 + x." ){
        et::var z = 15 + x;

        REQUIRE(z.getChildren().size() == 2);

        SECTION( "Require that the members are the same. (Not copied)"){
            REQUIRE(z.getChildren()[0].getValue() == 15);
            REQUIRE(z.getChildren()[1].getValue() == 10);
        }
    }

    SECTION( "Right add y + 20." ){
        et::var z = y + 15;

        REQUIRE(z.getChildren().size() == 2);

        SECTION( "Require that the members are the same. (Not copied)"){
            REQUIRE(z.getChildren()[0].getValue() == 20);
            REQUIRE(z.getChildren()[1].getValue() == 15);
        }
    }

    SECTION( "Observe that variables do not get destroyed when original out of scope." ){
        auto get_vars = []() -> et::var{
            et::var a(1), b(2), c(3), d(3);
            return (a+b)+(c+d);
        };
        et::var x = get_vars();
        REQUIRE(x.getChildren()[0].getChildren()[0].getValue() == 1);
        REQUIRE(x.getChildren()[1].getChildren()[1].getValue() == 3);
        REQUIRE(x.getChildren()[1].getChildren()[1].getUseCount() == 1);
    }
}

TEST_CASE( "et::var can be exponentiated.", "[et::var::exp]" ) {
    et::var y(10);
    et::var x = y + 10;

    SECTION( "Exponentiate an et::var." ){
        et::var z = et::exp(x);
        REQUIRE(z.getChildren().size() == 1);

        SECTION( "Require that the members are the same. (Not copied)" ){
            REQUIRE(z.getChildren()[0].getChildren()[0].getUseCount() == 2);
            REQUIRE(z.getChildren()[0].getChildren()[0].getValue() == 10);
        }
    }

    SECTION( "Observe that variables do not get destroyed when original out of scope." ){
        et::var* a = new et::var(1);
        et::var b = et::exp(*a);
        et::var x = et::exp(b);

        REQUIRE(a->getUseCount() == 2);
        
        delete a;

        REQUIRE(b.getChildren()[0].getUseCount() == 1); // a is child of b.
        REQUIRE(x.getChildren()[0].getChildren()[0].getUseCount() == 1);
    }
}

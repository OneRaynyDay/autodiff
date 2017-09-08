#include "catch.hpp"
#include "../src/etree.h"
#include <cmath>

#define NEW_CASE std::cout<<"======="<<std::endl;
#define NEW_SEC  std::cout<<"-------"<<std::endl;

// TEST_CASE( "et::expression can be initialized.", "[et::expression::expression]") {
    // et::expression exp(et::var(10));
    // REQUIRE(exp.getRoot().getValue() == 10);
//
    // SECTION( "et::expression's root will have correct children." ) {
        // et::var a(10), b(5), c(15), d(2);
        // et::var root = (a + b) + (c + d);
        // et::expression exp(root);
        // REQUIRE(exp.getRoot().getChildren()[0].getChildren()[0].getValue() == 10);
        // REQUIRE(exp.getRoot().getChildren()[0].getChildren()[1].getValue() == 5);
        // REQUIRE(exp.getRoot().getChildren()[1].getChildren()[0].getValue() == 15);
        // REQUIRE(exp.getRoot().getChildren()[1].getChildren()[1].getValue() == 2);
    // }
// }
//
// TEST_CASE( "et::expression can find all the leaves.", "[et::expression::findLeaves]") {
    // // although it is not required for the
    // // et::expression to find the nodes in this order
    // // we assert this for easier checking.
    // SECTION( "et::expression finds all sources in a+b+c+d" ){
        // et::var a(10), b(5), c(15), d(2);
        // et::var root = (a + b) + (c + d);
        // et::expression exp(root);
        // std::vector<et::var> v = exp.findLeaves();
        // REQUIRE(v.size() == 4);
        // REQUIRE(v[0].getValue() == 10);
        // REQUIRE(v[1].getValue() == 5);
        // REQUIRE(v[2].getValue() == 15);
        // REQUIRE(v[3].getValue() == 2);
    // }
//
    // SECTION( "et::expression finds all sources in exp(a) + b*3" ){
        // et::var a(10), b(5);
        // et::var root = et::exp(a) + b*3;
        // et::expression exp(root);
        // std::vector<et::var> v = exp.findLeaves();
        // REQUIRE(v.size() == 3);
        // REQUIRE(v[0].getValue() == 10);
        // REQUIRE(v[1].getValue() == 5);
        // REQUIRE(v[2].getValue() == 3);
    // }
// }
//
// TEST_CASE( "et::expression can evaluate an expression and return its value.", "[et::expression::propagate]") {
    // SECTION( "et::expression evaluates a+b+c+d" ) {
        // et::var a(10), b(5), c(15), d(2);
        // et::var root = (a + b) + (c + d);
        // et::expression exp(root);
        // REQUIRE(exp.propagate() == 32);
    // }
//
    // SECTION( "et::expression evaluates poly(a,b)/c" ) {
        // et::var a(2), b(3), c(8);
        // et::var root = et::poly(a,b) / c;
        // et::expression exp(root);
        // REQUIRE(exp.propagate() == 1);
    // }
//
    // SECTION( "et::expression evaluates exp(a) - b" ) {
        // et::var a(3), b(2.5);
        // et::var root = et::exp(a) - b;
        // et::expression exp(root);
        //
        // double val = std::exp(3) - 2.5;
        // REQUIRE(exp.propagate() == val);
    // }
// }

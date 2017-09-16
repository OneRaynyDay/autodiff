#include "catch.hpp"
#include "../src/utils.h"

#define NEW_CASE std::cout<<"======="<<std::endl;
#define NEW_SEC  std::cout<<"-------"<<std::endl;

TEST_CASE("et::eval can forward propagate.", "[et::eval]"){
    et::var x(2);
    et::var fx = et::poly(et::exp(3*x), 2) + 10;
    REQUIRE(et::eval(fx,true) - (std::exp(3*2)*std::exp(3*2)+10) < 1e-10);
}

TEST_CASE("et::back can back propagate.", "[et::back]"){
    et::var x(0.5);
    et::var fx = et::poly(et::exp(3*x + 1), 2.5)/10 + 10;
    std::unordered_map<et::var, double> m = {
        { x, 0 },
    };

    et::eval(fx, true);
    et::back(fx, m);
    
    REQUIRE(m[x] - ((3.0/4)*std::exp(7.5*0.5 + 2.5)) < 1e-10);
}

TEST_CASE("et::back can back propagate with nonconstqualify optimize.", "[et::back]"){
    et::var x(0.5);
    et::var fx = et::poly(et::exp(3*x + 1), 2.5)/10 + 10;
    std::unordered_map<et::var, double> m = {
        { x, 0 },
    };

    et::eval(fx, true);
    et::back(fx, m, {et::back_flags::const_qualify});
    
    REQUIRE(m[x] - ((3.0/4)*std::exp(7.5*0.5 + 2.5)) < 1e-10);
}

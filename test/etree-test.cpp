#include "catch.hpp"
#include "../src/etree.h"

#define NEW_CASE std::cout<<"======="<<std::endl;
#define NEW_SEC  std::cout<<"-------"<<std::endl;

TEST_CASE( "et::expression can be initialized." ) {
    et::expression(et::var(10));

}

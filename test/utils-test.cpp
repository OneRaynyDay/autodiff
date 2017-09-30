#include "catch.hpp"
#include "../src/utils.h"

#define NEW_CASE std::cout<<"======="<<std::endl;
#define NEW_SEC  std::cout<<"-------"<<std::endl;

using et::scalar;

TEST_CASE("et::eval can forward propagate.", "[et::eval]"){
    et::var x(2);
    et::var fx = et::poly(et::exp(3*x), 2) + 10;
    REQUIRE(et::eval(fx,true)(0,0) - (std::exp(3*2)*std::exp(3*2)+10) < 1e-10);
}

TEST_CASE("et::eval can forward propagate on matrices.", "[et::eval]"){
    et::var x(MatrixXd(MatrixXd::Ones(5,1)));
    et::var fx = et::poly(et::exp(et::dot(MatrixXd(MatrixXd::Ones(1,5)),x)), 2) + 10;
    REQUIRE(et::eval(fx,true)(0,0) - (std::exp(5*2)*std::exp(5*2)+10) < 1e-10);
}

TEST_CASE("et::back can back propagate.", "[et::back]"){
    et::var x(0.5);
    et::var fx = et::poly(et::exp(3*x + 1), 2.5)/10 + 10;
    std::unordered_map<et::var, MatrixXd> m = {
        { x, scalar(0) },
    };

    et::eval(fx, true);
    et::back(fx, m);

    REQUIRE(m[x](0,0) - ((3.0/4)*std::exp(7.5*0.5 + 2.5)) < 1e-10);
}

TEST_CASE("et::eval can back propagate on matrices. Linear Regression.", "[et::eval]"){
    // Linear regression has the loss function:
    // 1/N * sum[(Xw - y).^2]
    // Equivalent to: 1/N * (Xw - y)^T(Xw - y)
    MatrixXd _X(3,2); // 3 examples, 2 dimensions
    _X << 1, 2,
          3, 4,
          1, 3;
    MatrixXd _w(2,1); // 2 weights
    _w << 1,
          3;
    MatrixXd _y(3,1); // 3 labels
    _y << 2,
          3,
          2;
    et::var X(_X), w(_w), y(_y);
    et::var intermediate = et::dot(X, w) - y;
    et::var loss = et::dot(et::transpose(intermediate), intermediate);
    
    REQUIRE( et::eval(loss, true)(0,0) - (89.0+144) <= 1e-10 );

    auto expected_grad = 2 * _X.transpose() * (_X * _w - _y);
    std::unordered_map<et::var, MatrixXd> m = {
        { w, zeros_like(w) },
    };
    
    et::back(loss, m);
    
    std::cout << "Retrieved grad : " << m[w] << std::endl;
    std::cout << "Actual grad : " << expected_grad << std::endl;

    REQUIRE( m[w] == expected_grad );
}

TEST_CASE("et::back can back propagate with nonconstqualify optimize.", "[et::back]"){
    et::var x(0.5);
    et::var fx = et::poly(et::exp(3*x + 1), 2.5)/10 + 10;
    std::unordered_map<et::var, MatrixXd> m = {
        { x, scalar(0) },
    };

    et::eval(fx, true);
    et::back(fx, m, {et::back_flags::const_qualify});

    REQUIRE(m[x](0,0) - ((3.0/4)*std::exp(7.5*0.5 + 2.5)) < 1e-10);
}

TEST_CASE("et::eval can back propagate on matrices with nonconstqualify optimize. Linear Regression.", "[et::eval]"){
    // Linear regression has the loss function:
    // 1/N * sum[(Xw - y).^2]
    // Equivalent to: 1/N * (Xw - y)^T(Xw - y)
    MatrixXd _X(3,2); // 3 examples, 2 dimensions
    _X << 1, 2,
          3, 4,
          1, 3;
    MatrixXd _w(2,1); // 2 weights
    _w << 1,
          3;
    MatrixXd _y(3,1); // 3 labels
    _y << 2,
          3,
          2;
    et::var X(_X), w(_w), y(_y);
    et::var intermediate = et::dot(X, w) - y;
    et::var loss = et::dot(et::transpose(intermediate), intermediate);
    
    REQUIRE( et::eval(loss, true)(0,0) - (89.0+144) <= 1e-10 );

    auto expected_grad = 2 * _X.transpose() * (_X * _w - _y);
    std::unordered_map<et::var, MatrixXd> m = {
        { w, zeros_like(w) },
    };
    
    et::back(loss, m, {et::back_flags::const_qualify});
    
    std::cout << "Retrieved grad : " << m[w] << std::endl;
    std::cout << "Actual grad : " << expected_grad << std::endl;

    REQUIRE( m[w] == expected_grad );
}

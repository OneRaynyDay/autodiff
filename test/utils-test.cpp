#include "catch.hpp"
#include "../src/utils.h"
#include <typeinfo>
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

TEST_CASE("et::eval can back propagate on matrices. Linear Regression.", "[et::back]"){
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

TEST_CASE("et::eval can back propagate on matrices with nonconstqualify optimize. Linear Regression.", "[et::back]"){
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
    et::var loss = et::multiply(1.0/3 , et::dot(et::transpose(intermediate), intermediate));
    
    REQUIRE( et::eval(loss, true)(0,0) - (89.0+144)/3 <= 1e-10 );

    auto expected_grad = 2/3.0 * _X.transpose() * (_X * _w - _y);
    std::unordered_map<et::var, MatrixXd> m = {
        { w, zeros_like(w) },
    };
    
    et::back(loss, m, {et::back_flags::const_qualify});

    REQUIRE( m[w] == expected_grad );
}

TEST_CASE("et::eval can back propagate on matrices with nonconstqualify optimize. Logistic regression.", "[et::back]"){
    // Linear regression has the loss function:
    // 1/N * sum[(Xw - y).^2]
    // Equivalent to: 1/N * (Xw - y)^T(Xw - y)
    using et::divide;
    using et::multiply;
    using et::exp;
    using et::log;
    using et::subtract;
    using et::transpose;
    MatrixXd _X(3,2); // 3 examples, 2 dimensions
    _X << 1, 2,
          3, 4,
          1, 3;
    MatrixXd _w(2,1); // 2 weights
    _w << 1,
          -3;
    MatrixXd _y(3,1); // 3 labels
    _y << 1,
          0,
          1;
    et::var X(_X), w(_w), y(_y);
    et::var sigm = divide(1, add(1, exp(multiply(-1, et::dot(X, w)))));
    et::var loss = dot(transpose(y), log(sigm)) + dot(transpose(subtract(1, y)), log(subtract(1, sigm)));

    MatrixXd _A = _X*_w;
    MatrixXd _sigm = 1/(1+ ((-1)*_A.array()).exp());
    MatrixXd lhs = _y.transpose() * MatrixXd(_sigm.array().log());
    MatrixXd rhs = MatrixXd(1-_y.array()).transpose() * MatrixXd((1-_sigm.array()).log());
    MatrixXd _loss = lhs.array() + rhs.array();
    REQUIRE( et::eval(loss, true)(0,0) == _loss(0,0));

    MatrixXd expected_grad = _X.transpose() * MatrixXd(_y.array() - _sigm.array());
    std::unordered_map<et::var, MatrixXd> m = {
        { w, zeros_like(w) },
    };

    et::back(loss, m, {et::back_flags::const_qualify});

    REQUIRE( m[w] == expected_grad );
}

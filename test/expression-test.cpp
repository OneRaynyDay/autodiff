#include "catch.hpp"
#include "../src/expression.h"
#include <cmath>

#define NEW_CASE std::cout<<"======="<<std::endl;
#define NEW_SEC  std::cout<<"-------"<<std::endl;

using et::scalar;

TEST_CASE( "et::expression can be initialized.", "[et::expression::expression]") {
    et::expression exp(et::var(scalar(10)));
    REQUIRE(exp.getRoot().getValue() == scalar(10));

    SECTION( "et::expression's root will have correct children." ) {
        et::var a(scalar(10)), b(scalar(5)), c(scalar(15)), d(scalar(2));
        et::var root = (a + b) + (c + d);
        et::expression exp(root);
        REQUIRE(exp.getRoot().getChildren()[0].getChildren()[0].getValue() == scalar(10));
        REQUIRE(exp.getRoot().getChildren()[0].getChildren()[1].getValue() == scalar(5));
        REQUIRE(exp.getRoot().getChildren()[1].getChildren()[0].getValue() == scalar(15));
        REQUIRE(exp.getRoot().getChildren()[1].getChildren()[1].getValue() == scalar(2));
    }
}

TEST_CASE( "et::expression can find all the leaves.", "[et::expression::findLeaves]") {
    // although it is not required for the
    // et::expression to find the nodes in this order
    // we assert this for easier checking.
    SECTION( "et::expression finds all sources in a+b+c+d" ){
        et::var a(scalar(10)), b(scalar(5)), c(scalar(15)), d(scalar(2));
        et::var root = (a + b) + (c + d);
        et::expression exp(root);
        std::vector<et::var> v = exp.findLeaves();
        REQUIRE(v.size() == 4);
        REQUIRE(v[0].getValue()(0,0) + 
                v[1].getValue()(0,0) + 
                v[2].getValue()(0,0) + 
                v[3].getValue()(0,0) == 32);
    }

    SECTION( "et::expression finds all sources in exp(a) + b*3" ){
        et::var a(scalar(10)), b(scalar(5));
        et::var root = et::exp(a) + b*3;
        et::expression exp(root);
        std::vector<et::var> v = exp.findLeaves();
        REQUIRE(v.size() == 3);
        REQUIRE(v[0].getValue()(0,0) +  
                v[1].getValue()(0,0) +  
                v[2].getValue()(0,0) == 18);
    }
}

TEST_CASE( "et::expression can evaluate an expression *RECURSIVELY*.", "[et::expression::propagate]") {
    SECTION( "et::expression evaluates a+b+c+d" ) {
        et::var a(scalar(10)), b(scalar(5)), c(scalar(15)), d(scalar(2));
        et::var root = (a + b) + (c + d);
        et::expression exp(root);
        REQUIRE(exp.propagate() == scalar(32));
    }

    SECTION( "et::expression evaluates poly(a,b)/c" ) {
        et::var a(scalar(2)), b(scalar(3)), c(scalar(8));
        et::var root = et::poly(a,b) / c;
        et::expression exp(root);
        REQUIRE(exp.propagate() == scalar(1));
    }

    SECTION( "et::expression evaluates exp(a) - b" ) {
        et::var a(3), b(2.5);
        et::var root = et::exp(a) - b;
        et::expression exp(root);

        double val = std::exp(3) - 2.5;
        REQUIRE(exp.propagate() == scalar(val));
    }
}


TEST_CASE( "et::expression can evaluate an expression *ITERATIVELY*.", "[et::expression::propagate]") {
    SECTION( "et::expression evaluates a+b+c+d" ) {
        et::var a(10), b(5), c(15), d(2);
        et::var root = (a + b) + (c + d);
        et::expression exp(root);
        REQUIRE(exp.propagate(exp.findLeaves()) == scalar(32));
    }

    SECTION( "et::expression evaluates poly(a,b)/c" ) {
        et::var a(2), b(3), c(8);
        et::var root = et::poly(a,b) / c;
        et::expression exp(root);
        REQUIRE(exp.propagate(exp.findLeaves()) == scalar(1));
    }

    SECTION( "et::expression evaluates exp(a) - b" ) {
        et::var a(3), b(2.5);
        et::var root = et::exp(a) - b;
        et::expression exp(root);

        double val = std::exp(3) - 2.5;
        REQUIRE(exp.propagate(exp.findLeaves()) == scalar(val));
    }
}


TEST_CASE( "et::expression can find the derivatives.", "[et::expression::propagate]") {
    SECTION( "et::expression evaluates a+b+c+d" ) {
        et::var a(10), b(5), c(15), d(2);
        et::var root = (a + b) + (c + d);
        et::expression exp(root);
        std::unordered_map<et::var, MatrixXd> m = {
            { a, scalar(0) },
            { b, scalar(0) },
            { c, scalar(0) },
            { d, scalar(0) }
        };
        exp.propagate();
        exp.backpropagate(m);

        REQUIRE(m[a] == scalar(1));
        REQUIRE(m[b] == scalar(1));
        REQUIRE(m[c] == scalar(1));
        REQUIRE(m[d] == scalar(1));
    }

    SECTION( "et::expression evaluates poly(a,b)/c" ) {
        et::var a(2), b(3), c(8);
        et::var root = et::poly(a,b) / c;
        et::expression exp(root);
        std::unordered_map<et::var, MatrixXd> m = {
            { a, scalar(0) },
            { b, scalar(0) },
            { c, scalar(0) },
        };
        exp.propagate();
        exp.backpropagate(m);
        REQUIRE(m[a] == scalar(12.0/8));
        REQUIRE(m[b] == scalar(0));
        REQUIRE(m[c] == scalar((-8.0)/(64)));
    }

    SECTION( "et::expression evaluates exp(a) - b" ) {
        et::var a(3), b(2.5);
        et::var root = et::exp(a) - b;
        et::expression exp(root);

        std::unordered_map<et::var, MatrixXd> m = {
            { a, scalar(0) },
            { b, scalar(0) },
        };
        exp.propagate();
        exp.backpropagate(m);
        REQUIRE(m[a] == scalar(std::exp(3)));
        REQUIRE(m[b] == scalar(-1));
    }

    SECTION( "et::expression evaluates a*exp(a) - b" ) {
        et::var a(3), b(2.5);
        et::var root = a*et::exp(a) - b;
        et::expression exp(root);

        std::unordered_map<et::var, MatrixXd> m = {
            { a, scalar(0) },
            { b, scalar(0) },
        };
        exp.propagate();
        exp.backpropagate(m);
        REQUIRE(m[a] == scalar(std::exp(3) + std::exp(3)*3));
        REQUIRE(m[b] == scalar(-1));
    }

    SECTION( "et::expression evaluates log(A) + 3*log(A)" ) {
        MatrixXd _A(2,3);
        _A << 1,2,
              3,4,
              5,6;
        et::var A(_A);
        et::var root = et::log(A) + et::multiply(3, et::log(A));
        et::expression exp(root);

        std::unordered_map<et::var, MatrixXd> m = {
            { A, et::zeros_like(A) },
        };
        exp.propagate();
        exp.backpropagate(m);
        MatrixXd _grad = 4 / _A.array();
        std::cout << "expected GRAD : " << _grad << std::endl;
        REQUIRE(m[A] == _grad);
    }

    
    SECTION( "et::expression evaluates sigmoid(a)" ) {
        et::var a(3);
        et::var root = 1/(1+et::exp(-1*a));
        et::expression exp(root);

        std::unordered_map<et::var, MatrixXd> m = {
            { a, scalar(0) },
        };
        exp.propagate();
        exp.backpropagate(m);
        double sigm = 1/(1+std::exp(-3));
        double grad = sigm * (1-sigm);
        // precision is an issue here. So we make sure
        // that the diff in value isn't too large.
        REQUIRE(m[a](0,0)-grad < 1e-10);
    }
    
    SECTION( "et::expression evaluates sigmoid(A)" ) {
        MatrixXd _A(3,3);
        _A << 1,2,3,
              4,5,6,
              7,8,9;
        et::var A(_A);
        et::var root = et::divide(1, et::add(1, et::exp(et::multiply(-1, A))));
        et::expression exp(root);

        std::unordered_map<et::var, MatrixXd> m = {
            { A, et::zeros_like(A) },
        };
        MatrixXd val = exp.propagate();
        exp.backpropagate(m);
        MatrixXd _sigm = 1/(1+ ((-1)*_A.array()).exp());

        REQUIRE(val == _sigm);
        MatrixXd _grad = _sigm.array() * (1-_sigm.array());
        // precision is an issue here. So we make sure
        // that the diff in value isn't too large.
        REQUIRE((m[A].array()-_grad.array()).sum() < 1e-10);
    }
}

TEST_CASE( "et::expression can find the nonconsts correctly.", "[et::expression::findNonConsts]") {
    SECTION( "et::expression finds a, a+b, (a+b)+(c+d) in a+b+c+d" ) {
        et::var a(10), b(5), c(15), d(2);
        auto a_b = a+b;
        auto c_d = c+d;
        et::var root = a_b + c_d;
        et::expression exp(root);
        std::unordered_set<et::var> s = exp.findNonConsts({a});
        std::unordered_set<et::var> ans {a, a_b, root};
        REQUIRE(s == ans);
    }

    SECTION( "et::expression finds the values 2, 8, 1 in poly(a,b)/c" ) {
        et::var a(2), b(3), c(8);
        et::var root = et::poly(a,b) / c;
        et::expression exp(root);
        exp.propagate();

        std::unordered_set<et::var> s = exp.findNonConsts({a});
        std::unordered_set<double> vals;
        for(et::var v : s){
            vals.insert(v.getValue()(0,0));
        }

        std::unordered_set<double> ans {2, 8, 1};
        REQUIRE(vals == ans);
    }

    SECTION( "et::expression finds correct nonconsts in exp(a) - b + c * poly(d,e) + f - g" ) {
        et::var a(3), b(2.5), c(1), d(2), e(2), f(5), g(4);
        auto expa = et::exp(a);
        auto a_b = expa - b;
        auto d_e = et::poly(d, e);
        auto c_d_e = c * d_e;
        auto c_d_e_f_g = c_d_e + (f - g);
        et::var root = a_b + c_d_e_f_g;
        et::expression exp(root);

        std::unordered_set<et::var> s = exp.findNonConsts({a, d});
        std::unordered_set<et::var> ans {a, expa, a_b, d, d_e, c_d_e, c_d_e_f_g, root};
        REQUIRE(s.size() == ans.size());
    }
}

TEST_CASE( "et::expression can find the derivatives with nonconst optimizations.", "[et::expression::propagate]") {
    SECTION( "et::expression evaluates a+b+c+d" ) {
        et::var a(10), b(5), c(15), d(2);
        et::var root = (a + b) + (c + d);
        et::expression exp(root);
        std::unordered_map<et::var, MatrixXd> m = {
            { a, scalar(0) },
            { b, scalar(0) },
            { c, scalar(0) },
            { d, scalar(0) }
        };
        exp.propagate();
        std::unordered_set<et::var> s = exp.findNonConsts({a,b,c,d});
        exp.backpropagate(m, s);

        REQUIRE(m[a] == scalar(1));
        REQUIRE(m[b] == scalar(1));
        REQUIRE(m[c] == scalar(1));
        REQUIRE(m[d] == scalar(1));
    }

    SECTION( "et::expression evaluates poly(a,b)/c" ) {
        et::var a(2), b(3), c(8);
        et::var root = et::poly(a,b) / c;
        et::expression exp(root);
        std::unordered_map<et::var, MatrixXd> m = {
            { a, scalar(0) },
            { b, scalar(0) },
            { c, scalar(0) },
        };
        exp.propagate();
        std::unordered_set<et::var> s = exp.findNonConsts({a,b,c});
        exp.backpropagate(m, s);
        REQUIRE(m[a] == scalar((12.0/8)));
        REQUIRE(m[b] == scalar(0));
        REQUIRE(m[c] == scalar((-8.0)/(64)));
    }

    SECTION( "et::expression evaluates exp(a) - b" ) {
        et::var a(3), b(2.5);
        et::var root = et::exp(a) - b;
        et::expression exp(root);

        std::unordered_map<et::var, MatrixXd> m = {
            { a, scalar(0) },
            { b, scalar(0) },
        };
        exp.propagate();
        std::unordered_set<et::var> s = exp.findNonConsts({a,b});
        exp.backpropagate(m, s);
        REQUIRE(m[a] == scalar(std::exp(3)));
        REQUIRE(m[b] == scalar(-1));
    }

    SECTION( "et::expression evaluates a*exp(a) - b" ) {
        et::var a(scalar(3)), b(scalar(2.5));
        et::var root = a*et::exp(a) - b;
        et::expression exp(root);

        std::unordered_map<et::var, MatrixXd> m = {
            { a, scalar(0) },
            { b, scalar(0) },
        };
        exp.propagate();
        std::unordered_set<et::var> s = exp.findNonConsts({a,b});
        exp.backpropagate(m, s);
        REQUIRE(m[a] == scalar(std::exp(3) + std::exp(3)*3));
        REQUIRE(m[b] == scalar(-1));
    }

    SECTION( "et::expression evaluates sigmoid(a)" ) {
        et::var a(3);
        et::var root = 1/(1+et::exp(-1*a));
        et::expression exp(root);

        std::unordered_map<et::var, MatrixXd> m = {
            { a, scalar(0) },
        };
        exp.propagate();
        std::unordered_set<et::var> s = exp.findNonConsts({a});
        exp.backpropagate(m, s);
        double sigm = 1/(1+std::exp(-3));
        double grad = sigm * (1-sigm);
        // precision is an issue here. So we make sure
        // that the diff in value isn't too large.
        REQUIRE(m[a](0,0)-grad < 1e-10);
    }
}

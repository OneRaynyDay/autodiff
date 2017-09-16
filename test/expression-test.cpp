#include "catch.hpp"
#include "../src/expression.h"
#include <cmath>

#define NEW_CASE std::cout<<"======="<<std::endl;
#define NEW_SEC  std::cout<<"-------"<<std::endl;

TEST_CASE( "et::expression can be initialized.", "[et::expression::expression]") {
    et::expression exp(et::var(10));
    REQUIRE(exp.getRoot().getValue() == 10);

    SECTION( "et::expression's root will have correct children." ) {
        et::var a(10), b(5), c(15), d(2);
        et::var root = (a + b) + (c + d);
        et::expression exp(root);
        REQUIRE(exp.getRoot().getChildren()[0].getChildren()[0].getValue() == 10);
        REQUIRE(exp.getRoot().getChildren()[0].getChildren()[1].getValue() == 5);
        REQUIRE(exp.getRoot().getChildren()[1].getChildren()[0].getValue() == 15);
        REQUIRE(exp.getRoot().getChildren()[1].getChildren()[1].getValue() == 2);
    }
}

TEST_CASE( "et::expression can find all the leaves.", "[et::expression::findLeaves]") {
    // although it is not required for the
    // et::expression to find the nodes in this order
    // we assert this for easier checking.
    SECTION( "et::expression finds all sources in a+b+c+d" ){
        et::var a(10), b(5), c(15), d(2);
        et::var root = (a + b) + (c + d);
        et::expression exp(root);
        std::vector<et::var> v = exp.findLeaves();
        REQUIRE(v.size() == 4);
        REQUIRE(v[0].getValue() == 10);
        REQUIRE(v[1].getValue() == 5);
        REQUIRE(v[2].getValue() == 15);
        REQUIRE(v[3].getValue() == 2);
    }

    SECTION( "et::expression finds all sources in exp(a) + b*3" ){
        et::var a(10), b(5);
        et::var root = et::exp(a) + b*3;
        et::expression exp(root);
        std::vector<et::var> v = exp.findLeaves();
        REQUIRE(v.size() == 3);
        REQUIRE(v[0].getValue() == 10);
        REQUIRE(v[1].getValue() == 5);
        REQUIRE(v[2].getValue() == 3);
    }
}

TEST_CASE( "et::expression can evaluate an expression *RECURSIVELY*.", "[et::expression::propagate]") {
    SECTION( "et::expression evaluates a+b+c+d" ) {
        et::var a(10), b(5), c(15), d(2);
        et::var root = (a + b) + (c + d);
        et::expression exp(root);
        REQUIRE(exp.propagate() == 32);
    }

    SECTION( "et::expression evaluates poly(a,b)/c" ) {
        et::var a(2), b(3), c(8);
        et::var root = et::poly(a,b) / c;
        et::expression exp(root);
        REQUIRE(exp.propagate() == 1);
    }

    SECTION( "et::expression evaluates exp(a) - b" ) {
        et::var a(3), b(2.5);
        et::var root = et::exp(a) - b;
        et::expression exp(root);

        double val = std::exp(3) - 2.5;
        REQUIRE(exp.propagate() == val);
    }
}


TEST_CASE( "et::expression can evaluate an expression *ITERATIVELY*.", "[et::expression::propagate]") {
    SECTION( "et::expression evaluates a+b+c+d" ) {
        et::var a(10), b(5), c(15), d(2);
        et::var root = (a + b) + (c + d);
        et::expression exp(root);
        REQUIRE(exp.propagate(exp.findLeaves()) == 32);
    }

    SECTION( "et::expression evaluates poly(a,b)/c" ) {
        et::var a(2), b(3), c(8);
        et::var root = et::poly(a,b) / c;
        et::expression exp(root);
        REQUIRE(exp.propagate(exp.findLeaves()) == 1);
    }

    SECTION( "et::expression evaluates exp(a) - b" ) {
        et::var a(3), b(2.5);
        et::var root = et::exp(a) - b;
        et::expression exp(root);

        double val = std::exp(3) - 2.5;
        REQUIRE(exp.propagate(exp.findLeaves()) == val);
    }
}


TEST_CASE( "et::expression can find the derivatives.", "[et::expression::propagate]") {
    SECTION( "et::expression evaluates a+b+c+d" ) {
        et::var a(10), b(5), c(15), d(2);
        et::var root = (a + b) + (c + d);
        et::expression exp(root);
        std::unordered_map<et::var, double> m = {
            { a, 0 },
            { b, 0 },
            { c, 0 },
            { d, 0 }
        };
        exp.propagate();
        exp.backpropagate(m);

        REQUIRE(m[a] == 1);
        REQUIRE(m[b] == 1);
        REQUIRE(m[c] == 1);
        REQUIRE(m[d] == 1);
    }

    SECTION( "et::expression evaluates poly(a,b)/c" ) {
        et::var a(2), b(3), c(8);
        et::var root = et::poly(a,b) / c;
        et::expression exp(root);
        std::unordered_map<et::var, double> m = {
            { a, 0 },
            { b, 0 },
            { c, 0 },
        };
        exp.propagate();
        exp.backpropagate(m);
        REQUIRE(m[a] == (12.0/8));
        REQUIRE(m[b] == 0);
        REQUIRE(m[c] == (-8.0)/(64));
    }

    SECTION( "et::expression evaluates exp(a) - b" ) {
        et::var a(3), b(2.5);
        et::var root = et::exp(a) - b;
        et::expression exp(root);

        std::unordered_map<et::var, double> m = {
            { a, 0 },
            { b, 0 },
        };
        exp.propagate();
        exp.backpropagate(m);
        REQUIRE(m[a] == std::exp(3));
        REQUIRE(m[b] == -1);
    }

    SECTION( "et::expression evaluates a*exp(a) - b" ) {
        et::var a(3), b(2.5);
        et::var root = a*et::exp(a) - b;
        et::expression exp(root);

        std::unordered_map<et::var, double> m = {
            { a, 0 },
            { b, 0 },
        };
        exp.propagate();
        exp.backpropagate(m);
        REQUIRE(m[a] == std::exp(3) + std::exp(3)*3);
        REQUIRE(m[b] == -1);
    }

    SECTION( "et::expression evaluates sigmoid(a)" ) {
        et::var a(3);
        et::var root = 1/(1+et::exp(-1*a));
        et::expression exp(root);

        std::unordered_map<et::var, double> m = {
            { a, 0 },
        };
        exp.propagate();
        exp.backpropagate(m);
        double sigm = 1/(1+std::exp(-3));
        double grad = sigm * (1-sigm);
        // precision is an issue here. So we make sure
        // that the diff in value isn't too large.
        REQUIRE(m[a]-grad < 1e-10);
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
            vals.insert(v.getValue());
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
        std::unordered_map<et::var, double> m = {
            { a, 0 },
            { b, 0 },
            { c, 0 },
            { d, 0 }
        };
        exp.propagate();
        std::unordered_set<et::var> s = exp.findNonConsts({a,b,c,d});
        exp.backpropagate(m, s);

        REQUIRE(m[a] == 1);
        REQUIRE(m[b] == 1);
        REQUIRE(m[c] == 1);
        REQUIRE(m[d] == 1);
    }

    SECTION( "et::expression evaluates poly(a,b)/c" ) {
        et::var a(2), b(3), c(8);
        et::var root = et::poly(a,b) / c;
        et::expression exp(root);
        std::unordered_map<et::var, double> m = {
            { a, 0 },
            { b, 0 },
            { c, 0 },
        };
        exp.propagate();
        std::unordered_set<et::var> s = exp.findNonConsts({a,b,c});
        exp.backpropagate(m, s);
        REQUIRE(m[a] == (12.0/8));
        REQUIRE(m[b] == 0);
        REQUIRE(m[c] == (-8.0)/(64));
    }

    SECTION( "et::expression evaluates exp(a) - b" ) {
        et::var a(3), b(2.5);
        et::var root = et::exp(a) - b;
        et::expression exp(root);

        std::unordered_map<et::var, double> m = {
            { a, 0 },
            { b, 0 },
        };
        exp.propagate();
        std::unordered_set<et::var> s = exp.findNonConsts({a,b});
        exp.backpropagate(m, s);
        REQUIRE(m[a] == std::exp(3));
        REQUIRE(m[b] == -1);
    }

    SECTION( "et::expression evaluates a*exp(a) - b" ) {
        et::var a(3), b(2.5);
        et::var root = a*et::exp(a) - b;
        et::expression exp(root);

        std::unordered_map<et::var, double> m = {
            { a, 0 },
            { b, 0 },
        };
        exp.propagate();
        std::unordered_set<et::var> s = exp.findNonConsts({a,b});
        exp.backpropagate(m, s);
        REQUIRE(m[a] == std::exp(3) + std::exp(3)*3);
        REQUIRE(m[b] == -1);
    }

    SECTION( "et::expression evaluates sigmoid(a)" ) {
        et::var a(3);
        et::var root = 1/(1+et::exp(-1*a));
        et::expression exp(root);

        std::unordered_map<et::var, double> m = {
            { a, 0 },
        };
        exp.propagate();
        std::unordered_set<et::var> s = exp.findNonConsts({a});
        exp.backpropagate(m, s);
        double sigm = 1/(1+std::exp(-3));
        double grad = sigm * (1-sigm);
        // precision is an issue here. So we make sure
        // that the diff in value isn't too large.
        REQUIRE(m[a]-grad < 1e-10);
    }
}

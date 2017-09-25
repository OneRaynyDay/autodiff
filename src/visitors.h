#pragma once

#include "var.h"

namespace et{

Because we're using boost::variant, we need to consider a 

class plus_visitor : public boost::static_visitor<term_t> {
public:
    term_t operator()(double lhs, double rhs) const{
        return {lhs + rhs};
    }
    term_t operator()(double lhs, VectorXd rhs) const{
        return {lhs + rhs};
    }
    term_t operator()(double lhs, MatrixXd rhs) const{
        return {lhs + rhs};
    }
    term_t operator()(VectorXd lhs, VectorXd rhs) const{
        return {lhs + rhs};
    }
};


}

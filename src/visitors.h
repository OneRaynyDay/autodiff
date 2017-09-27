#pragma once

#include "var.h"
#include <boost/variant.hpp>

namespace et{
class var_visitor : public boost::static_visitor<term_type> {
    term_type operator()(auto& t) {
        using T = typename std::decay<decltype(t)>::type;
        if constexpr(std::is_same<T,double>::value ) {
            return term_type::double_t;
        }
        else if constexpr(std::is_same<T,VectorXd>::value ) {
            return term_type::vector_t;
        }
        else if constexpr(std::is_same<T,MatrixXd>::value ) {
            return term_type::matrix_t;
        }
        else return term_type::unknown_t;
    } 
};


class plus_visitor : public boost::static_visitor<term_t> {
public:
    term_t operator()(double lhs, double rhs) const{
        return {double(lhs + rhs)};
    }
    term_t operator()(double lhs, const VectorXd& rhs) const{
        return {VectorXd(lhs + rhs.array())};
    }
    term_t operator()(double lhs, const MatrixXd& rhs) const{
        return {MatrixXd(lhs + rhs.array())};
    }
    term_t operator()(const VectorXd& lhs, double rhs) const{
        return operator()(rhs, lhs);
    }
    term_t operator()(const VectorXd& lhs, const VectorXd& rhs) const{
        return {VectorXd(lhs + rhs)};
    }
    term_t operator()(const VectorXd& lhs, const MatrixXd& rhs) const{
        // Vector + Matrix will always be rowwise additions.
        // For example:
        // >>> y = np.ones((5,))
        // >>> x = np.ones((3,5))
        // >>> x + y
        // array([[ 2.,  2.,  2.,  2.,  2.],
        //        [ 2.,  2.,  2.,  2.,  2.],
        //        [ 2.,  2.,  2.,  2.,  2.]])
        return {MatrixXd(rhs.rowwise() + lhs.transpose())};
    }
    term_t operator()(const MatrixXd& lhs, double rhs) const{
        return operator()(rhs, lhs);
    }
    term_t operator()(const MatrixXd& lhs, const VectorXd& rhs) const{
        return operator()(rhs, lhs);
    }
    term_t operator()(const MatrixXd& lhs, const MatrixXd& rhs) const{
        return {MatrixXd(lhs + rhs)};
    }
};



}




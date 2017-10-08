#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <utility>
#include <eigen3/Eigen/Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::ArrayXd;

namespace et{
// forward declare class var
class var;

// Current support for operators:
// operator+
// operator-
// operator*
// operator/
// exp() // e^x
// poly() // x^n
enum class op_type {
    plus,
    minus,
    multiply,
    divide,
    exponent,
    log,
    polynomial,
    dot,
    inverse,
    transpose,
    sum,
    none // no operators. leaf.
};

int numOpArgs(op_type op);

// Convenience functions for initializing MatrixXd's.
MatrixXd scalar(double);
MatrixXd zeros_like(const MatrixXd&);
MatrixXd zeros_like(const var&);
MatrixXd ones_like(const MatrixXd&);
MatrixXd ones_like(const var&);

}

namespace std{
// Template specialize hash for vars
template <> struct hash<et::var> {
    size_t operator()(const et::var&) const;
};
}

namespace et{
// A genius debugging tool, made by milleniumbug.
template<typename T>
struct noisy
{
	noisy& operator=(noisy&&) noexcept { std::cout << "operator=(noisy<" << typeid(T).name() << ">&&)\n"; return *this; }
	noisy& operator=(const noisy&) { std::cout << "operator=(const noisy<" << typeid(T).name() << ">&)\n"; return *this; }
	noisy(const noisy&) { std::cout << "noisy(const noisy<" << typeid(T).name() << ">&)\n"; }
	noisy(noisy&&) noexcept { std::cout << "noisy(noisy<" << typeid(T).name() << ">&&)\n"; }
	~noisy() { std::cout << "~noisy<" << typeid(T).name() << ">()\n"; }
	noisy() { std::cout << "noisy<" << typeid(T).name() << ">()\n"; }
};


/** 
 * ::Example 2::
 *
 * et::var x(10);
 * et::var y = x + 10;
 * et::var z(0);
 *
 * eval(z); // outputs 0.
 *
 * y.children.size(); // outputs 1.
 *
 * z = x + y; // z gets assignment operator'd!
 *
 * z.children.size(); // outputs 2. We will not implement path compression.
 * eval(z); // z's original value is overwritte; outputs 30
 *
 * y.val; // outputs 20. It's already evaluated by eval(z)! 
 */

class var {
// Forward declaration
struct impl;

public:
    // For initialization of new vars by ptr
    var(std::shared_ptr<impl>);

    var(double);
    var(const MatrixXd&);
    var(op_type, const std::vector<var>&);
    ~var();

    // movable
    var(var&&) noexcept;
    var& operator=(var&&) noexcept;
    
    // shallow copyable
    var(const var&);
    var& operator=(const var&);

    // deep copyable
    var clone();
    
    // Access/Modify the current node value
    MatrixXd getValue() const;
    void setValue(const MatrixXd&);
    op_type getOp() const;
    void setOp(op_type);
    
    // Access internals (no modify)
    
    // We return by reference because we do not
    // want to increase the shared_ptr count.
    // (Even though it's innocuous so far)
    std::vector<var>& getChildren() const;

    // We return by copy here because we want
    // to let the user know that the shared_ptr
    // count is increased for the duration that
    // the parents are held.
    std::vector<var> getParents() const;
    long getUseCount() const;

    // Comparison for hash
    bool operator==(const var& rhs) const;
    friend struct std::hash<var>;

    // Arithmetic expressions
    // We expose this friend purely because we don't want to expose the pimpl
    // using functions. We shouldn't be using friends often.
    template <typename... V>
    friend const var pack_expression(op_type, V&...);
private: 
    // PImpl idiom requires forward declaration of the class:
    std::shared_ptr<impl> pimpl;
};

struct var::impl{
public:
    // Either allow to enter a value(leaf)
    // Or allow to enter operation and children(parent)
    impl(const MatrixXd&);
    impl(op_type, const std::vector<var>&);

    // The value that the variable currently holds.
    // Currently only supports MatrixXd.
    // In the future template and type promotion should be
    // taken into consideration.
    MatrixXd val;

    // The operator associated with this variable.
    // For example, `z = x + y` will have z contain
    // an op value of var::op::plus
    op_type op; 

    // The children of the current variable, 
    // i.e. which variables make up this variable.
    std::vector<var> children;

    // TODO: Currently the API supports pointing weak_ptrs.
    // This way, we won't have the issue with shared_ptr loops.
    // We should devise a cleaner way to do this if possible.
    std::vector<std::weak_ptr<impl>> parents;
};

// Inline definitions of templated functions:
template <typename... V>
const var pack_expression(op_type op, V&... args){
    std::vector<std::shared_ptr<var::impl> > vimpl = { args.pimpl... };
    std::vector<var> v;
    for(const std::shared_ptr<var::impl>& _impl : vimpl){
        v.emplace_back(_impl); 
    }
    var res(op, v);
    for(const std::shared_ptr<var::impl>& _impl : vimpl){
        _impl->parents.push_back(res.pimpl);
    }
    return res;
}

// We need const-ness in returns here to prevent things like:
// a + b = c; which is obviously dumb

inline const var operator+(var lhs, var rhs){
    return pack_expression(op_type::plus, lhs, rhs);
}

inline const var operator-(var lhs, var rhs){
    return pack_expression(op_type::minus, lhs, rhs);
}

inline const var operator*(var lhs, var rhs){
    return pack_expression(op_type::multiply, lhs, rhs);
}

inline const var operator/(var lhs, var rhs){
    return pack_expression(op_type::divide, lhs, rhs);
}

inline const var exp(var v){
    return pack_expression(op_type::exponent, v);
}

inline const var log(var v){
    return pack_expression(op_type::log, v);
}

inline const var poly(var v, var power){
    return pack_expression(op_type::polynomial, v, power);
}

inline const var dot(var lhs, var rhs){
    return pack_expression(op_type::dot, lhs, rhs);
}

inline const var inverse(var v){
    return pack_expression(op_type::inverse, v);
}

inline const var transpose(var v){
    return pack_expression(op_type::transpose, v);
}

inline const var sum(var v){
    return pack_expression(op_type::sum, v);
}

}


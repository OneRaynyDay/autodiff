#pragma once

// Macros for debugging
#define DEBUG

#ifdef DEBUG
 #define D if(1) 
#else
 #define D if(0) 
#endif
// enddebug

#include <iostream>
#include <vector>
#include <memory>
#include <utility>

namespace et{

/** 
 * The var class is the core, lightweight wrapper
 * that allows automatic differentiation.
 *
 * Some use cases for var is the following:
 *
 * ::Example 1::
 *
 * et::var x(10), y(20), z(5);
 *
 * std::vector<et::var> args {x,y,z};
 *
 * // returns a number
 * et::eval(x);
 * // returns a var
 * (x + y);
 * // returns a var
 * (y + z * 5);
 * // returns a var
 * final = (3 + x^2 - 1);
 * // returns a number
 * et::eval(final);
 * // returns the gradient with respect to x, is a et::var
 * auto dx = et::back(final, x);
 * // returns a std::map<etc::var, etc::var> m{ {x, dx}, {y, dy}, {z, dz} };
 * auto grad = et::back(final, args);
 *
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

class var : noisy<var>{
public:
    // Current support for operators:
    // operator+
    // operator-
    // operator*
    // operator/
    // exp() // e^x
    // poly() // x^n
    enum op_type {
        plus,
        minus,
        multiply,
        divide,
        exponent,
        polynomial,
        none // no operators. leaf.
    };

    // ctor: loads in val, no children.
    var(double);
    // ctor: loads in operator, and children. Note: children is an rvalue pass.
    var(op_type, std::vector<std::shared_ptr<var>>&&);
    // copy ctor: loads in val, shallow copies children.
    // var(const var&) = default;
    
    // Access the current value of the node.
    double getValue() const{ return val; };
    std::vector<std::shared_ptr<var>> getChildren() const{
        return children; 
    }

    // assignment op: does a _shallow_ copy of children.
    // this way, the following expressions:
    // `z = x + y` 
    // `z = 10`
    // `z = 10.5`
    // `z = some_matrix_type()`
    // ... will be valid during evaluation.
    // var& operator=(const var& rhs) = default;

    // ~ Binary Operators ~
    // These are the most important operator
    // overloading functions for our use case.
    // These are _globally defined_ operator overloads.
    //
    // Implementation details:
    // - Currently is _lazily evaluated_.
    //  - The reasoning is "create once, evaluate often".
    //
    // NOTE: Only make them friend if you _absolutely_
    // need to access private members! 
    friend var operator+(const var& lhs, const var& rhs);
    friend var operator-(const var& lhs, const var& rhs);
    friend var operator*(const var& lhs, const var& rhs);
    friend var operator/(const var& lhs, const var& rhs);
    friend var exp(const var&);
    friend var poly(const var&, double power);

private: 
        
    // The value that the variable currently holds.
    // Currently only supports double.
    // In the future template and type promotion should be
    // taken into consideration.
    double val;

    // The operator associated with this variable.
    // For example, `z = x + y` will have z contain
    // an op value of var::op::plus
    op_type op; 

    // The children of the current variable.
    // A children is defined by the following:
    // A child c_i is a child if the expression E containing
    // c_i and current node v evaluates c_i before v
    // is evaluated.
    std::vector<std::shared_ptr<var> > children;
};

// Inline definitions of templated functions:
template <typename... V>
var pack_expression(var::op_type op, const V&... args){
    return var(op,
               std::vector<std::shared_ptr<var> > {
                   std::make_shared<var>(args)...,
               });
}

inline var operator+(const var& lhs, const var& rhs){
    return pack_expression(var::op_type::plus, lhs, rhs);
}

inline var operator-(const var& lhs, const var& rhs){
    return pack_expression(var::op_type::minus, lhs, rhs);
}

inline var operator*(const var& lhs, const var& rhs){
    return pack_expression(var::op_type::multiply, lhs, rhs);
}

inline var operator/(const var& lhs, const var& rhs){
    return pack_expression(var::op_type::divide, lhs, rhs);
}

inline var exp(const var& v){
    return pack_expression(var::op_type::exponent, v);
}

inline var poly(const var& v, double power){
    return pack_expression(var::op_type::polynomial, v, power);
}
}

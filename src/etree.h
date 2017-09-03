#pragma once

#include <vector>
#include <memory>
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
class var{
public:
    // ctor: loads in val, no children.
    var(double val);
    // copy ctor: loads in val, deep copies children.
    var(const var& rhs);
    
    // Access the current value of the node.
    double value() const{ return val; };

    // assignment op: does a shallow copy of children.
    // this way, the following expressions:
    // `z = x + y` 
    // `z = 10`
    // `z = 10.5`
    // `z = some_matrix_type()`
    // ... will be valid during evaluation.
    // TODO: TEMPORARILY TURNED OFF FOR NOW
    // template <typename T>
    // var& operator=(const T& rhs);

    // ~ Binary Operators ~
    // These are the most important operator
    // overloading functions for our use case.
    // These are _globally defined_ operator overloads.
    //
    // NOTE: Only make them friend if you _absolutely_
    // need to access private members! 
    // friend var operator+(const var& lhs, const var& rhs);
    friend var operator+(const var& lhs, const var& rhs);
    template <typename T>
    friend var operator+(const var& lhs, const T& rhs);
    template <typename T>
    friend var operator+(const T& lhs, const var& rhs);

private: 
    // Current support for operators:
    // operator+
    // operator-
    // operator*
    // operator/
    // exp() // e^x
    // poly() // x^n
    enum operators{
        plus,
        minus,
        multiply,
        divide,
        exponent,
        polynomial
    };
    
    // The value that the variable currently holds.
    // Currently only supports double.
    // In the future template and type promotion should be
    // taken into consideration.
    double val;

    // The children of the current variable.
    // A children is defined by the following:
    // A child c_i is a child if the expression E containing
    // c_i and current node v evaluates c_i before v
    // is evaluated.
    std::vector<var> children; 
};

// Inline definitions of templated functions:

// Explicit specialization:
inline var operator+(const var& lhs, const var& rhs){
    return var(lhs.value() + rhs.value());
}

template <typename T>
var operator+(const var& lhs, const T& rhs){
    return var(lhs.val + rhs);
}

template <typename T>
var operator+(const T& lhs, const var& rhs){
    return var(rhs.val + lhs);
}


}

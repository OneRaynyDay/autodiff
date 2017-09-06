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

class var;
typedef std::shared_ptr<var> vsp;

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
    polynomial,
    none // no operators. leaf.
};

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

class var : noisy<var> {
// Forward declaration
struct impl;

public:
    // For initialization of new vars by ptr
    var(std::shared_ptr<impl>);

    var(double);
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
    
    // Access the current value of the node.
    double getValue() const;
    const std::vector<var>& getChildren() const;
    long getUseCount() const;

    template <typename... V>
    friend var pack_expression(op_type, V&...);

    friend var operator+(var lhs, var rhs);
    friend var operator-(var lhs, var rhs);
    friend var operator*(var lhs, var rhs);
    friend var operator/(var lhs, var rhs);
    friend var exp(var);
    friend var poly(var, double power);
private: 
    // PImpl idiom requires forward declaration of the class:
    std::shared_ptr<impl> pimpl;
};

struct var::impl{
public:
    // Either allow to enter a value(leaf)
    // Or allow to enter operation and children(parent)
    impl(double);
    impl(op_type, const std::vector<var>&);
    
    // The value that the variable currently holds.
    // Currently only supports double.
    // In the future template and type promotion should be
    // taken into consideration.
    double val;

    // The operator associated with this variable.
    // For example, `z = x + y` will have z contain
    // an op value of var::op::plus
    op_type op; 

    // The children of the current variable, 
    // i.e. which variables make up this variable.
    std::vector<var> children;

    // TODO: Currently the API supports pointing weak_ptrs.
    // We should devise a cleaner way to do this if possible.
    std::vector<std::weak_ptr<impl>> parents;
};

// Inline definitions of templated functions:
template <typename... V>
var pack_expression(op_type op, V&... args){
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

inline var operator+(var lhs, var rhs){
    return pack_expression(op_type::plus, lhs, rhs);
}

inline var operator-(var lhs, var rhs){
    return pack_expression(op_type::minus, lhs, rhs);
}

inline var operator*(var lhs, var rhs){
    return pack_expression(op_type::multiply, lhs, rhs);
}

inline var operator/(var lhs, var rhs){
    return pack_expression(op_type::divide, lhs, rhs);
}

inline var exp(var v){
    return pack_expression(op_type::exponent, v);
}

inline var poly(var v, double power){
    var p(power);
    return pack_expression(op_type::polynomial, v, p);
}

}

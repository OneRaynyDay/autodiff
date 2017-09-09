#pragma once

#include "expression.h"

namespace et{

// The utils file is a list of functions that
// could be commonly used by the user.
//
// So far, we support eval(), and back().
//
// The general format is for the user to
// input a specific flag into the functions.
// The user _should not_ have to know the internals.

// Provides an interface for the et::expression evaluation
// pipeline. This is to abstract away the construction of
// an expression and choose the method of evaluation.
double eval(var& v, bool iter);

// Provides an interface for the et::expression backprop
// pipeline.
void back(const var&, std::unordered_map<var, double>&);

}

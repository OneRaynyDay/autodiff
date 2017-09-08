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

double eval(const var&);

std::unordered_map<var, double> back(const var&, std::unordered_map<var, double>&);
}

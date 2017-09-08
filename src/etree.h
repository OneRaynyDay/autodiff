#pragma once

#include "var.h"
#include <queue>
#include <unordered_map>

namespace et{
/**
 * The expression class is a wrapper over a variable that
 * contains many children that will soon be evaluated.
 * It follows the "facade" design pattern.
 *
 * We will use the expression class mainly to:
 * - find the leaves of the expression tree
 * - TODO: premature optimization of the expression tree.
 *      - i.e.: x + x + x + x => 3*x
 * 
 * We will use the expression class inside of:
 * - et::eval()
 * - et::back()
 */
class expression {
public:
    expression(var);

    var getRoot() const;

    // Searches for the leaves of the DAG.
    // returns in a std::vector to evaluate for later.
    std::vector<var> findLeaves();

    /** TODO: discussion:
     * Do we really need propagate()? Can the user just
     * evaluate it forward themselves, and update the leaves
     * with appropriate values?
     */

    // Recursively evaluates the tree.
    // This may have memory issues if the stack size is significant.
    double propagate();
    
    // Uses the given leaves, possibly from findSource(),
    // and performs a bottom-up evaluation of the tree
    // from the leaves.
    double propagate(const std::vector<var>& leaves);

    // Computes the derivative for the entire graph.
    // Performs a top-down evaluation of the tree.
    //
    void backpropagate(std::unordered_map<var, double>& leaves);
    
private:
    var root;
};

}

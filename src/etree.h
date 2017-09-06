#pragma once

#include "var.h"
#include <queue>

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

    // Searches for the leaves of the DAG.
    // returns in a std::vector to evaluate for later.
    std::vector<var> findSource();
private:
    var root;
};

expression::expression(var _root) : root(_root){}

std::vector<var> expression::findSource(){
    std::vector<var> leaves;
    std::queue<var> q;
    q.push(root);

    while(!q.empty()){
        var v = q.front();
        if(!v.getChildren().size())
            leaves.push_back(v);
        else{
            std::vector<var> children = v.getChildren();
            for(const var& v : children){
                q.push(v);
            }
        }
        q.pop();
    }
    return leaves;
}

}

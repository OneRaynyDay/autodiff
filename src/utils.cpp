#include "utils.h"

namespace et{

MatrixXd eval(var& root, bool iter){
    expression exp(root);
    if(iter)
        return exp.propagate(exp.findLeaves());
    else
        return exp.propagate();
}

void back(const var& root, 
        std::unordered_map<var, MatrixXd>& derivative){
    expression exp(root);
    exp.backpropagate(derivative);
}

void back(expression& exp, 
        std::unordered_map<var, MatrixXd>& derivative,
        const std::unordered_set<var>& leaves)
{
    exp.backpropagate(derivative, leaves);
}

}

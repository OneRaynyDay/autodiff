#include "utils.h"

namespace et{

double eval(var& root, bool iter){
    expression exp(root);
    if(iter)
        return exp.propagate(exp.findLeaves());
    else
        return exp.propagate();
}

void back(const var& root, std::unordered_map<var, double>& derivative){
     expression exp(root);
     exp.backpropagate(derivative);
}


}

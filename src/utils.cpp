#include "utils.h"

namespace et{

double eval(var& root, bool iter){
    expression exp(root);
    if(iter)
        return exp.propagate(exp.findLeaves());
    else
        return exp.propagate();
}

void back(const var& root, 
        std::unordered_map<var, double>& derivative,
        std::set<back_flags> flags){
    expression exp(root);
    if(flags.find(back_flags::const_qualify) != flags.end()){
        std::vector<var> leaves;
        for(auto p : derivative){
            leaves.push_back(p.first); 
        }
        std::unordered_set<var> s = exp.findNonConsts(leaves);
        exp.backpropagate(derivative, s);
    }
    else{ 
        exp.backpropagate(derivative);
    }
}


}

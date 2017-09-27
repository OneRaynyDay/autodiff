#include "expression.h"
#include "visitors.h"
#include <cmath>
#include <exception>

namespace et{

// Helper function for recursive propagation
term_t _eval(op_type op, const std::vector<var>& operands){
    switch(op){
        case op_type::plus:
            return boost::apply_visitor( eval_plus_visitor(), operands[0].getTerm(), operands[1].getTerm() );
        case op_type::minus:
            return boost::apply_visitor( eval_minus_visitor(), operands[0].getTerm(), operands[1].getTerm() );
        // case op_type::multiply:
            // return operands[0].getValue() * operands[1].getValue();
        // case op_type::divide:
            // return operands[0].getValue() / operands[1].getValue();
        // case op_type::exponent:
            // return std::exp(operands[0].getValue());
        // case op_type::polynomial:
            // return std::pow(operands[0].getValue(), operands[1].getValue());
        // case op_type::none:
        default:
            throw std::invalid_argument("Cannot have a non-leaf contain none-op.");
    }; 
}

// Helper function for recursive backpropagation
double _back_single(op_type op, 
        const std::vector<var>& operands,
        int op_idx){
    throw std::invalid_argument("Backpropagate not yet implemented");
    return 0;
    // switch(op){
        // case op_type::plus: {
            // return 1;
        // }
        // case op_type::minus: {
            // if(op_idx == 0)
                // return 1;
            // else
                // return -1;
        // }
        // case op_type::multiply: {
            // return operands[(1-op_idx)].getValue();
        // }
        // case op_type::divide: {
            // if(op_idx == 0)
                // return 1 / operands[1].getValue();
            // else
                // return -operands[0].getValue() / std::pow(operands[1].getValue(), 2);
        // }
        // case op_type::exponent: {
            // return std::exp(operands[0].getValue());
        // }
        // case op_type::polynomial: {
            // if(op_idx == 0)
                // return std::pow(operands[0].getValue(), operands[1].getValue()-1) *
                    // operands[1].getValue();
            // else
                // return 0; // we don't support exponents other than e.
        // }
        // case op_type::none: {
            // throw std::invalid_argument("Cannot have a non-leaf contain none-op.");
        // }
    // };
}

std::vector<double> _back(op_type op, const std::vector<var>& operands,
        const std::vector<bool>& nonconsts,
        double dx){
    std::vector<double> derivatives;
    for(size_t i = 0; i < operands.size(); i++){
        if(!nonconsts[i])
            derivatives.push_back(0); // no gradient flow.
        else
            derivatives.push_back(dx * _back_single(op, operands, i));
    }
    return derivatives;
}

std::vector<double> _back(op_type op, const std::vector<var>& operands,
        double dx){
    std::vector<double> derivatives;
    for(size_t i = 0; i < operands.size(); i++){
        derivatives.push_back(dx * _back_single(op, operands, i));
    }
    return derivatives;
}

expression::expression(var _root) : root(_root){}

var expression::getRoot() const{
    return root;
}

std::vector<var> expression::findLeaves(){
    std::vector<var> leaves;
    std::queue<var> q;
    q.push(root);

    while(!q.empty()){
        var v = q.front();
        if(v.getChildren().empty())
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

void _rpropagate(var& v){
    if(v.getChildren().empty())
        return;
    std::vector<var> children = v.getChildren(); 
    for(var& _v : children){
        _rpropagate(_v);
    }
    v.setValue(_eval(v.getOp(), v.getChildren()));
}

term_t expression::propagate(){
    _rpropagate(root);
    return root.getValue();
}

// This one is a little tougher:
// We want to create a queue for which can be parallelized;
// A node is "available" if it has all of its children loaded.
// Currently, we wrap around the vars, and add a field
// that tracks how many occurences of its children we found.
//
// Pseudocode:
// suppose q has all the leaves, and m contains <var, int>
// while q not empty:
//     v = q.pop
//     evaluate v
//     for parent in v.parents:
//         m[parent]++
//         if m[parent] has sufficient nums:
//             q.add(parent)
//     
// return root.val

term_t expression::propagate(const std::vector<var>& leaves){
    std::queue<var> q;
    std::unordered_map<var, int> explored; 
    for(const var& v : leaves)
        q.push(v);

    while(!q.empty()){
        var v = q.front();
        q.pop();
        std::vector<var> parents = v.getParents();
        for(var& parent : parents){
            explored[parent]++; 
            if(numOpArgs(parent.getOp()) == explored[parent]){
                parent.setValue(_eval(parent.getOp(), parent.getChildren()));
                q.push(parent);
            }
        } 
    } 
    return root.getValue();
}

std::unordered_set<var> expression::findNonConsts(const std::vector<var>& leaves){
    std::unordered_set<var> nonconsts;
    std::queue<var> q; 
    for(const var& v : leaves)
        q.push(v); 

    while(!q.empty()){
        var v = q.front();
        q.pop();
        
        // We should not traverse this if it has already been visited.
        if(nonconsts.find(v) != nonconsts.end())
            continue;
        
        nonconsts.insert(v);
        std::vector<var> parents = v.getParents();
        for(const var& parent : parents){
            q.push(parent);
        }
    }
    return nonconsts;
}

// When we perform a BFS to find the derivatives, we should be careful not to
// 1. override its original value.
//     x = a + b
//     y = a + c
//     derivative of a is dx/da + dy/da
// 2. explore too much of the tree that is not unnecessary.
//     Ideally, our user would be smart and input a value directly rather than
//     create an entire expression subtree for a value that is a constant.
//     TODO: In the future, add a field in var that states whether it's a constant.

void expression::backpropagate(std::unordered_map<var, double>& leaves){
    std::queue<var> q;
    std::unordered_map<var, double> derivatives;
    q.push(root);
    derivatives[root] = 1;
    
    while(!q.empty()){
        var v = q.front();
        q.pop();
        std::vector<var>& children = v.getChildren();
        std::vector<double> child_derivs = _back(v.getOp(), children, derivatives[v]);
        for(size_t i = 0; i < children.size(); i++){
            // Be careful to not override the derivative value!
            derivatives[children[i]] += child_derivs[i];
            if(children[i].getOp() != op_type::none)
                q.push(children[i]); 
        }
    }
   
    // After we have retrieved the derivatives,
    // select the leaves and update in leaves.
    for(auto& iter : leaves){
        iter.second = derivatives[iter.first]; 
    } 
}

// Restricted BFS: same as previous, but we will have a set of nonconsts to tell us
// where we can BFS to.
void expression::backpropagate(std::unordered_map<var, double>& leaves, 
        const std::unordered_set<var>& nonconsts){
    std::queue<var> q;
    std::unordered_map<var, double> derivatives;
    q.push(root);
    derivatives[root] = 1;
    
    while(!q.empty()){
        var v = q.front();
        q.pop();
        if(nonconsts.find(v) == nonconsts.end())
            continue;
        std::vector<var>& children = v.getChildren();
        std::vector<double> child_derivs = _back(v.getOp(), children, derivatives[v]);
        for(size_t i = 0; i < children.size(); i++){
            // Be careful to not override the derivative value!
            derivatives[children[i]] += child_derivs[i];
            if(children[i].getOp() != op_type::none)
                q.push(children[i]); 
        }
    }
   
    // After we have retrieved the derivatives,
    // select the leaves and update in leaves.
    for(auto& iter : leaves){
        iter.second = derivatives[iter.first]; 
    } 
}

}

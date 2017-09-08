#include "etree.h"
#include <cmath>
#include <exception>
#include <unordered_map>
#include <iostream>

namespace et{

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

// Helper function for recursive propagation
double _eval(op_type op, std::vector<var> operands){
    switch(op){
        case op_type::plus:
            return operands[0].getValue() + operands[1].getValue();
        case op_type::minus:
            return operands[0].getValue() - operands[1].getValue();
        case op_type::multiply:
            return operands[0].getValue() * operands[1].getValue();
        case op_type::divide:
            return operands[0].getValue() / operands[1].getValue();
        case op_type::exponent:
            return std::exp(operands[0].getValue());
        case op_type::polynomial:
            return std::pow(operands[0].getValue(), operands[1].getValue());
        case op_type::none:
            throw std::invalid_argument("Cannot have a non-leaf contain none-op.");
    }; 
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

double expression::propagate(){
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

double expression::propagate(const std::vector<var>& leaves){
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

}

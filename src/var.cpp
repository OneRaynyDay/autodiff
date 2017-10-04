#include "var.h"
#include <map>

namespace et{

MatrixXd scalar(double num){
    MatrixXd m(1,1);
    m(0,0) = num;
    return m;
}

MatrixXd zeros_like(const MatrixXd& like){
    return MatrixXd::Zero(like.rows(), like.cols());
}

MatrixXd zeros_like(const var& like){
    return MatrixXd::Zero(like.getValue().rows(), like.getValue().cols());
}

MatrixXd ones_like(const MatrixXd& like){
    return MatrixXd::Ones(like.rows(), like.cols());
}

MatrixXd ones_like(const var& like){
    return MatrixXd::Ones(like.getValue().rows(), like.getValue().cols());
}

/* enum find */

int numOpArgs(op_type op){
    static const std::map<op_type, int> op_args = {
        { op_type::plus, 2 },
        { op_type::minus, 2 },
        { op_type::multiply, 2 },
        { op_type::divide, 2 },
        { op_type::exponent, 1 },
        { op_type::log, 1 },
        { op_type::polynomial, 2 },
        { op_type::dot, 2 },
        { op_type::inverse, 1 },
        { op_type::transpose, 1 },
        { op_type::none, 0 },
    };
    return op_args.find(op)->second;
};

/* et::var default funcs: */

// movable
var::var(var&&) noexcept = default;
var& var::operator=(var&&) noexcept = default;
var::~var() = default;
// shallow copyable
var::var(const var&) = default;
var& var::operator=(const var&) = default;
// deep copyable
var var::clone(){
    return var(std::make_shared<impl>(*pimpl)); 
}

/* et::var funcs: */
var::var(std::shared_ptr<impl> _pimpl) : pimpl(_pimpl){};

var::var(const MatrixXd& _val) 
: pimpl(new impl(_val)){}

var::var(double _val) 
: pimpl(new impl(scalar(_val))){}

var::var(op_type _op, const std::vector<var>& _children)
: pimpl(new impl(_op, _children)){}

/* getters and setters */
MatrixXd var::getValue() const{ return pimpl->val; }

void var::setValue(const MatrixXd& _val){ pimpl->val = _val; }

op_type var::getOp() const{ return pimpl->op; }

void var::setOp(op_type _op){ pimpl->op = _op; }

std::vector<var>& var::getChildren() const{ return pimpl->children; }

std::vector<var> var::getParents() const{
    std::vector<var> _parents;
    for( std::weak_ptr<impl> parent : pimpl->parents ){
        _parents.emplace_back( parent.lock() );
    } 
    return _parents;
}

long var::getUseCount() const{
    return pimpl.use_count();
}

/* hash/comparisons */
bool var::operator==(const var& rhs) const{ return pimpl.get() == rhs.pimpl.get(); }

/* et::var::impl funcs: */
var::impl::impl(const MatrixXd& _val) : 
    val(_val), 
    op(op_type::none){}

var::impl::impl(op_type _op, const std::vector<var>& _children)
: op(_op) {
    for(const var& v : _children){
        children.emplace_back(v.pimpl);
    }
}

}

namespace std{

// Template specialize hash for vars
size_t hash<et::var>::operator()(const et::var& v) const{
    return std::hash<std::shared_ptr<et::var::impl> >{}(v.pimpl);
}

}

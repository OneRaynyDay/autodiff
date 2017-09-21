#include "var.h"
#include <map>

namespace et{
/* enum find */

int numOpArgs(op_type op){
    static const std::map<op_type, int> op_args = {
        { op_type::plus, 2 },
        { op_type::minus, 2 },
        { op_type::multiply, 2 },
        { op_type::divide, 2 },
        { op_type::exponent, 1 },
        { op_type::polynomial, 1 },
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

// Ctor
var::var(double _val) 
: pimpl(std::make_shared<impl>(_val)){}
var::var(VectorXd _val)
: pimpl(std::make_shared<impl>(_val)){}
var::var(MatrixXd _val)
: pimpl(std::make_shared<impl>(_val)){}

var::var(op_type _op, const std::vector<var>& _children)
: pimpl(std::make_shared<impl>(_op, _children)){}

/* getters and setters */

void var::setValue(double _val) { 
    pimpl->val = std::make_shared<double>(_val); 
    pimpl->term = term_type::scalar; 
}

void var::setValue(const VectorXd& _val) { 
    pimpl->val = std::make_shared<VectorXd>(_val); 
    pimpl->term = term_type::vector; 
}

void var::setValue(const MatrixXd& _val) { 
    pimpl->val = std::make_shared<MatrixXd>(_val); 
    pimpl->term = term_type::matrix; 
}

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
var::impl::impl(double _val) : 
    val(std::make_shared<double>(_val)), 
    op(op_type::none),
    term(term_type::scalar){}
var::impl::impl(VectorXd _val) : 
    val(std::make_shared<VectorXd>(_val)), 
    op(op_type::none),
    term(term_type::vector){}
var::impl::impl(MatrixXd _val) : 
    val(std::make_shared<MatrixXd>(_val)), 
    op(op_type::none),
    term(term_type::matrix){}

var::impl::impl(op_type _op, const std::vector<var>& _children)
: op(_op), term(term_type::none) {
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

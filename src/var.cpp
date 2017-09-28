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
var var::clone() const{
    return var(std::make_shared<impl>(*pimpl)); 
}

/* et::var funcs: */
var::var(std::shared_ptr<impl> _pimpl) : pimpl(_pimpl){};

// Ctor
var::var(double _val)
: pimpl(std::make_shared<impl>(_val)){}

var::var(MatrixXd _val)
: pimpl(std::make_shared<impl>(_val)){}

var::var(op_type _op, const std::vector<var>& _children)
: pimpl(std::make_shared<impl>(_op, _children)){}

/* getters and setters */
term_t var::getValue() const { return pimpl->val; }

void var::setValue(term_t _val) { 
    pimpl->val = _val; 
}

op_type var::getOp() const{ return pimpl->op; }

void var::setOp(op_type _op){ pimpl->op = _op; }

const term_t& var::getTerm() const{ return pimpl->val; }

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
var::impl::impl(term_t _val): 
    val(_val),
    op(op_type::none){}

var::impl::impl(op_type _op, const std::vector<var>& _children)
: op(_op){
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

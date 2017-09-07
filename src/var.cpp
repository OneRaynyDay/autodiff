#include "var.h"

namespace et{

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

var::var(double _val) 
: pimpl(new impl(_val)){}

var::var(op_type _op, const std::vector<var>& _children)
: pimpl(new impl(_op, _children)){}

/* getters and setters */
double var::getValue() const{ return pimpl->val; }
void var::setValue(double _val){ pimpl->val = _val; }
op_type var::getOp() const{ return pimpl->op; }
void var::setOp(op_type _op){ pimpl->op = _op; }
std::vector<var> var::getChildren() const{ return pimpl->children; }
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
var::impl::impl(double _val) : val(_val), op(op_type::none){}

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

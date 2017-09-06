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

double var::getValue() const{ 
    return pimpl->val;
}

const std::vector<var>& var::getChildren() const{
    return pimpl->children; 
}

long var::getUseCount() const{
    return pimpl.use_count();
}

/* et::var::impl funcs: */
var::impl::impl(double _val) : val(_val), op(op_type::none){}

var::impl::impl(op_type _op, const std::vector<var>& _children)
: op(_op) {
    for(const var& v : _children){
        children.emplace_back(v.pimpl);
    }
}

}

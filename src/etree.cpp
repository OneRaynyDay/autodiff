#include "etree.h"

namespace et{

var::var(double _val) : val(_val){
};

var::var(var::op_type _op, std::vector<std::shared_ptr<var>>&& _children) : op(_op), children(_children)
{};

}

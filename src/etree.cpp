#include "etree.h"

namespace et{

var::var(double _val) : val(_val){};

var::var(const var& rhs){
    // iterate by value
    for(var m_child : rhs.children){
        // recursively copy all children.
        children.push_back( m_child );
    };
};

}

//
// Created by a404m on 9/11/22.
//

#include "Variable.h"
#include <felan/compiler/make_package/MakePackage.h>

namespace felan {

    Variable::Variable(Node &node, MakePackage *mp, Parent _parent) :
    attribute(node),
    name(node.operands.front().str),
    type(mp->findClass(node.operands[1].str)),
    parent(_parent) {
        if(type == nullptr){
            throw NotFoundError("no type found by name of "+node.operands[1].str);
        }
    }

    Variable::Variable(std::string _name) : name(std::move(_name)){
        //empty
    }

    Variable::~Variable() {
        //empty
    }

    bool Variable::isIncomplete() const {
        return parent.kind == Parent::NONE;
    }

    bool operator==(const Variable &var1, const Variable &var2) {
        return var1.name == var2.name;
    }
} // felan
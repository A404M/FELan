//
// Created by a404m on 9/25/22.
//

#include "Parent.h"
#include <felan/common/package/Package.h>
#include <felan/common/fun/Fun.h>
#include <felan/common/class/Class.h>

namespace felan {
    Parent::Parent(void *_pointer, Parent::Kind _kind) : pointer(_pointer), kind(_kind) {
        //empty
    }

    Parent::Parent(Package *package) : pointer(package), kind(PACKAGE){
        //empty
    }

    Parent::Parent(Class *clas) : pointer(clas), kind(CLASS){
        //empty
    }

    Parent::Parent(Fun *fun) : pointer(fun), kind(FUN) {
        //empty
    }

    Parent::Parent() : pointer(nullptr), kind(NONE) {
        //empty
    }

    Parent Parent::getParent() const {
        switch(this->kind){
            case PACKAGE:
                return ((Package*)this->pointer)->father;
            case CLASS:
                return ((Class*)this->pointer)->father;
            case FUN:
                return ((Fun*)this->pointer)->parent;
            case NONE:
            default:
                throw std::runtime_error("bad kind");
        }
    }

    std::string Parent::getName() const {
        switch(this->kind){
            case PACKAGE:
                return ((Package*)this->pointer)->name;
            case CLASS:
                return ((Class*)this->pointer)->name;
            case FUN:
                return ((Fun*)this->pointer)->name;
            case NONE:
            default:
                throw std::runtime_error("bad kind");
        }
    }
} // felan
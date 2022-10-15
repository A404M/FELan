//
// Created by a404m on 9/12/22.
//

#include "Package.h"
#include <felan/common/fun/Fun.h>
#include <felan/common/class/Class.h>
#include <algorithm>
#include <utility>


namespace felan {
    Package::Element::Element(void *_pointer, Package::Element::Kind _kind,bool _owns)
    : pointer(_pointer), kind(_kind), owns(_owns){
        //empty
    }

    Package::Element::~Element() {
        if(!owns)
            return;
        switch(kind){
            case CLASS:
                delete (Class *)pointer;
                break;
            case FUN:
                delete (Fun *)pointer;
                break;
            case VARIABLE:
                delete (Variable *)pointer;
                break;
            case PACKAGE:
                delete (Package *)pointer;
                break;
            case NONE:
                break;
            default:
                throw std::runtime_error("pointer is broken");
        }
    }

    Package::Element::Element(Package::Element &&element) noexcept {
        this->pointer = element.pointer;
        this->kind = element.kind;
        this->owns = element.owns;
        element.kind = NONE;
        element.owns = false;
    }

    bool Package::Element::isIncomplete() const {
        switch(this->kind){
            case CLASS:
                return this->getAs<Class>().isIncomplete();
            case FUN:
                return this->getAs<Fun>().isIncomplete();
            case VARIABLE:
                return this->getAs<Variable>().isIncomplete();
            case PACKAGE:
                return false;
            case NONE:
            default:
                throw std::runtime_error("unexpected kind");
        }
    }

    std::string_view Package::Element::getName() const {
        switch(this->kind){
            case CLASS:
                return ((Class*)this->pointer)->name;
            case FUN:
                return ((Fun*)this->pointer)->name;
            case VARIABLE:
                return ((Variable*)this->pointer)->name;
            case PACKAGE:
                return ((Package*)this->pointer)->name;
            case NONE:
            default:
                throw std::runtime_error("can't get name of broken Element");
        }
    }

    bool Package::Element::equals(void *ptr, Package::Element::Kind kind) const {
        if(this->kind == kind){
            switch(kind){
                case CLASS:
                    return *(Class*)this->pointer==*(Class*)ptr;
                case FUN:
                    return *(Fun*)this->pointer==*(Fun*)ptr;
                case VARIABLE:
                    return *(Variable*)this->pointer==*(Variable*)ptr;
                case PACKAGE:
                    return *(Package*)this->pointer==*(Package*)ptr;
                case NONE:
                default:
                    throw std::runtime_error("broken kind");
            }
        }
        return false;
    }

    Package::Package(std::string _name, Package *_father, std::vector<Element> _elements) : name(std::move(_name)),
    father(_father,_father!=nullptr?Parent::PACKAGE:Parent::NONE), elements(std::move(_elements)){
        //empty
    }

    Package *Package::pushPackage(std::string packageName) {
        auto pointer = find(packageName, Element::PACKAGE);
        if(pointer == nullptr) {
            auto p = new Package(std::move(packageName), this);
            elements.emplace_back(p, Element::PACKAGE,true);
            return p;
        }
        return (Package*)pointer;
    }

    template<typename Func>
    Class *Package::pushClass(Node &node,Func stringToClass) {
        auto p = new Class(node,stringToClass);
        elements.emplace_back(p, Element::CLASS,true);
        return p;
    }

    void *Package::push(void *pointer, Package::Element::Kind kind,bool owns) {
        this->elements.emplace_back(pointer,kind,owns);
        return this->elements.back().pointer;
    }

    void *Package::pushInComplete(std::string eName, Package::Element::Kind kind) {
        void *p = nullptr;
        switch(kind){
            case Element::CLASS:
                p = new Class(std::move(eName));
                break;
            case Element::FUN:
                p = new Fun(std::move(eName));
                break;
            case Element::VARIABLE:
                p = new Variable(std::move(eName));
                break;
            case Element::PACKAGE:
            case Element::NONE:
            default:
                throw std::runtime_error("unexpected kind");
        }
        this->elements.emplace_back(p,kind,true);
        return p;
    }

    Package::Element *Package::find(void *pointer, Package::Element::Kind kind) {
        auto it = std::find_if(elements.begin(), elements.end(),
                               [pointer,kind](const Element &element) {
                                   return element.equals(pointer,kind);
                               }
        );
        return (it!=elements.end())?it.base(): nullptr;
    }

    void *Package::find(std::string_view elName, Element::Kind kind) {
        auto it = std::find_if(elements.begin(), elements.end(),
                           [&elName,kind](const Element &element) {
                               return element.kind == kind &&
                                      elName == element.getName();
                           }
        );
        return (it!=elements.end())?it.base()->pointer: nullptr;
    }

    Package::Element *Package::findAny(const std::string_view elName) {
        auto it = std::find_if(elements.begin(), elements.end(),
                               [elName](const Element &element) {
                                   return elName == element.getName();
                               }
        );
        return (it!=elements.end())?it.base(): nullptr;
    }

    Class *Package::findInAnyLevel(const std::string &className,Element::Kind kind) {
        Class *result = nullptr;
        for(auto &element : this->elements){
            if(element.kind == kind){
                if(element.getAs<Class>().name == className){
                    if(result != nullptr)
                        throw std::runtime_error(className+" has multiple definition");
                    result = (Class*)element.pointer;
                }
            }else if(element.kind == Element::PACKAGE){
                auto p = element.getAs<Package>().findInAnyLevel(className,kind);
                if(p != nullptr) {
                    if(result != nullptr)
                        throw std::runtime_error(className+" has multiple definition");
                    return p;
                }
            }
        }
        return result;
    }

    Package::Element *Package::getByPath(const std::string &path, Package::Element::Kind kind) {
        auto it = std::find(path.begin(),path.end(),'.');
        if(it != path.end()){
            std::string str = {path.begin(),it};
            auto pack = (Package*)this->find(str,Element::PACKAGE);
            auto clas = (Class*)this->find(str,Element::CLASS);
            if(pack != nullptr){
                if(clas != nullptr){
                    throw std::runtime_error("multiple finding");
                }
                return pack->getByPath({it+1,path.end()},kind);
            }else if(clas != nullptr){
                return clas->findAny({it+1,path.end()});
            }else{
                throw NotFoundError("no "+str+" found");
            }
        }

        auto p = this->findAny(path);
        if(p == nullptr || p->kind == kind)
            return p;
        else
            return nullptr;
    }

    bool Package::hasPackage(const std::string &packageName) {
        return std::find_if(elements.begin(), elements.end(), [&pName = std::as_const(packageName)](const Element &element){
            return element.kind == Element::PACKAGE && pName == element.getAs<Package>().name;
        }) != elements.end();
    }

    bool Package::hasClass(const std::string &className) {
        return std::find_if(elements.begin(), elements.end(), [&cName = std::as_const(className)](const Element &element){
            return element.kind == Element::CLASS && element.getAs<Class>().name == cName;
        }) != elements.end();
    }

    bool Package::hasIncomplete() {
        return std::find_if(elements.begin(), elements.end(), [](const Element &element){
            return element.isIncomplete();
        }) != elements.end();
    }

    Package *Package::findIncompletePackage(Package *pack) {
        if(pack->hasIncomplete())
            return pack;

        for(auto &element : pack->elements){
            if(element.kind == Element::PACKAGE){
                auto p = findIncompletePackage((Package*)element.pointer);
                if(p != nullptr){
                    return p;
                }
            }
        }

        return nullptr;
    }

    bool operator==(const Package &pack1, const Package &pack2) {
        return pack1.name == pack2.name;
    }

} // felan
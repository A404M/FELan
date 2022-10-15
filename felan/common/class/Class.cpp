//
// Created by a404m on 9/12/22.
//

#include "Class.h"
#include <algorithm>

namespace felan {

    Class::Class(Node &node, MakePackage *mp, Package *_father) :
            attribute(node),
            name(std::move(node.operands.front().str)),
            father(_father) {
        bodyNodes = std::move(node.operands.back().operands);
        node.operands.clear();
    }

    Class::Class(std::string _name) : name(std::move(_name)) {
        //empty
    }

    void Class::makeBody(MakePackage *mp) {
        for(auto &n : bodyNodes){
            switch(n.sToken){
                case Node::ST_CLASS:
                    throw SyntaxError("no class allowed inside class");
                case Node::ST_FUN:
                    this->members.emplace_back(
                            new Fun(n,mp,Parent(this)),
                            Package::Element::FUN,
                            true);
                    break;
                case Node::ST_VAR:
                    this->members.emplace_back(
                            new Variable(n,mp,Parent(this)),
                            Package::Element::VARIABLE,
                            true);
                    break;
                case Node::ST_FROM:
                case Node::ST_NONE:
                case Node::ST_EOL:
                case Node::ST_LEFT:
                case Node::ST_RIGHT:
                case Node::ST_BOTH:
                case Node::ST_BETWEEN:
                case Node::ST_FUN_CALL:
                default:
                    throw SyntaxError("no "+n.str+" allowed inside class");
            }
        }
        this->bodyNodes.clear();
    }

    void Class::completeBody(MakePackage *mp) {
        for(auto &el : this->members){
            switch(el.kind){
                case Package::Element::FUN:
                    ((Fun *) el.pointer)->completeBody(mp);
                    break;
                case Package::Element::VARIABLE:
                case Package::Element::NONE:
                case Package::Element::CLASS:
                case Package::Element::PACKAGE:
                    break;
            }
        }
    }

    bool Class::isIncomplete() const {
        return father.kind == Parent::NONE;
    }

    Fun *Class::getMethod(std::string_view str, std::vector<Class *> arguments) {
        auto f = Fun(std::string(str));
        f.arguments = std::move(arguments);
        auto it = std::find_if(this->members.begin(),this->members.end(),
                               [&f](const Package::Element &member){
            return member.kind == Package::Element::FUN &&
            f == *(Fun*)member.pointer;
        });
        return it!=this->members.end()?(Fun*)it->pointer:nullptr;
    }

    Package::Element *Class::findAny(std::string_view elName) {
        auto it = std::find_if(this->members.begin(),this->members.end(),
                             [elName](const Package::Element &member){
            return elName == member.getName();
        });
        return it != this->members.end()?&*it:nullptr;
    }

    bool operator==(const Class &clas1, const Class &clas2) {
        return clas1.name == clas2.name;
    }

} // felan
//
// Created by a404m on 9/12/22.
//

#include "Fun.h"
#include <felan/compiler/make_package/MakePackage.h>
#include <felan/common/expression/Expression.h>
#include <algorithm>
#include <felan/common/class/Class.h>

namespace felan {

    Fun Fun::assign("__assign__");

    Fun::Fun(Node &node, MakePackage *mp,Parent _parent) :
    attribute(node),
    name(std::move(node.operands.front().str)),
    vars("", nullptr),
    parent(_parent){

        auto it = node.operands.begin()+1;
        auto &params = it->operands;
        paramSize = params.size();
        for(auto &param : params){
            if(param.sToken != Node::ST_VAR){
                throw SyntaxError("params only can be vars");
            }
            auto varP = new Variable(param,mp,Parent(this));
            vars.push(varP,Package::Element::VARIABLE,true);
            arguments.emplace_back(varP->type);
        }
        ++it;

        if(it == node.operands.end()){
            goto END;
        }

        if(it->generalEquals(Node::BL_CURLY_BRACKET_OPEN)){
            retType = nullptr;
            goto BODY;
        }else{
            retType = mp->findClass(it->str);

            if(++it == node.operands.end()){
                goto END;
            }
        }

        BODY:
        if(it != node.operands.end())
            bodyNode = std::move(*it);
        else
            bodyNode = Node("",Node::T_NONE);

        ++it;
        END:
        if(this->name == "__constructor__"){
            if(this->retType != nullptr) {
                throw SyntaxError("constructor doesn't have return type");
            }else{
                if (parent.kind == Parent::CLASS) {
                    retType = static_cast<Class *>(parent.pointer);
                } else {
                    throw SyntaxError("__constructor__ is only for in classes functions or methods");
                }
            }
        }

        if(it != node.operands.end()){
            throw SyntaxError("bad fun syntax");
        }
    }

    Fun::Fun(std::string _name) :
    name(std::move(_name)),
    vars("", nullptr){
        //empty
    }

    Fun::~Fun() {
        //as far as vars is package there is no need to delete manually
        /*for(auto &var : this->vars){
            delete var;
        }*/
    }

    bool Fun::isIncomplete() const {
        return parent.kind == Parent::NONE;
    }

    void Fun::completeBody(MakePackage *mp) {
        auto &bodyLines = bodyNode.operands;
        for(auto &line : bodyLines){
            this->body.emplace_back(line,mp,this);
        }
        bodyNode.operands.clear();
    }

    Variable *Fun::addVar(Node &node, MakePackage *mp) {
        auto varP = new Variable(node,mp,Parent(this));
        /*for(const auto &variable : this->vars){
            if(variable->name == varP->name){
                std::string temp = std::move(varP->name);
                delete varP;
                throw SyntaxError("multiple definition of variable "+temp);
            }
        }*/
        if(this->vars.find(varP,Package::Element::VARIABLE)){
            std::string temp = std::move(varP->name);
            delete varP;
            throw SyntaxError("multiple definition of variable "+temp);
        }
        //return this->vars.emplace_back(varP);
        return (Variable*)this->vars.push(varP,Package::Element::VARIABLE,true);
    }

    Package::Element *Fun::findVar(std::string_view varName){
        /*for(auto &var : this->vars){
            if(varName == var->name){
                return var;
            }
        }
        return nullptr;*/
        return this->vars.findAny(varName);
    }

    int64_t Fun::getVarID(Variable *var) {
        for(int64_t i = 0;i < this->vars.elements.size();++i){
            if(this->vars.elements[i].pointer == var){
                return i;
            }
        }
        return -1;
    }

    bool operator==(const Fun &f1, const Fun &f2) {
        return f1.name == f2.name &&
               f1.arguments == f2.arguments;
    }
} // felan
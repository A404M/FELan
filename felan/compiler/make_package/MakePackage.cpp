//
// Created by a404m on 9/14/22.
//

#include "MakePackage.h"
#include <felan/compiler/compiler_frontend/CompilerFrontend.h>
#include <algorithm>
#include <functional>
#include <felan/common/class/Class.h>
#include <felan/common/expression/Expression.h>

namespace felan {
    Package MakePackage::rootPackage{"", nullptr};

    MakePackage::MakePackage(Parser _parser, const std::string& fileName, CompilerFrontend *_cf)
    : cf(_cf), globalElements("",nullptr), parser(std::move(_parser)){
        doAll(fileName);
    }

    Class *MakePackage::findClass(std::string_view str) {
        auto temp = (Class*)findGlobal(str,Package::Element::CLASS);
        if(temp == nullptr)
            throw NotFoundError("no class found");
        return temp;
    }

    Fun *MakePackage::findFun(std::string_view str, std::vector<Class *> arguments) {
        auto f = Fun(std::string(str));
        f.arguments = std::move(arguments);
        auto end = this->globalElements.elements.end();
        auto it = std::find_if(this->globalElements.elements.begin(),end,
                               [&f](const Package::Element &element){
            return element.kind == Package::Element::FUN &&
            f == *(Fun*)element.pointer;
        });
        if(it == end)
            throw std::runtime_error("no fun found");
        return (Fun*)it->pointer;
    }

    Variable *MakePackage::findVar(std::string_view str) {
        auto temp = (Variable*)findGlobal(str,Package::Element::VARIABLE);
        if(temp == nullptr)
            throw std::runtime_error("no variable found");
        return temp;
    }

    void MakePackage::doAll(const std::string& fileName) {
        if(this->parser.holder.empty()){
            return;
        }

        //do package
        auto it = this->parser.holder.begin();
        if(it->generalEquals(Node::KW_PACKAGE)) {
            this->package = doPackage(*it, fileName);
            parser.holder.erase(it);
        }else{
            this->package = rootPackage.pushPackage(fileName);
        }

        //this will do importing package itself job
        doAllClasses();
        doImports();

        doMake();
    }

    void MakePackage::doAllClasses() {
        for(auto it = this->parser.holder.begin(),
                end = this->parser.holder.end();it < end;){
            if(it->sToken == Node::ST_CLASS){
                doClass(*it,*package);
                it = parser.holder.erase(it);
                --end;
            }else{
                ++it;
            }
        }
    }

    void MakePackage::doMake() {
        for(auto it = this->parser.holder.begin(),
                end = this->parser.holder.end();it < end;){
            switch(it->sToken){
                case Node::ST_CLASS:
                    doClass(*it,*package);
                    break;
                case Node::ST_FUN:
                    doFun(*it,*package);
                    break;
                case Node::ST_VAR:
                    doVariable(*it,*package);
                    break;
                case Node::ST_LEFT:
                case Node::ST_RIGHT:
                case Node::ST_BOTH:
                case Node::ST_BETWEEN:
                case Node::ST_FUN_CALL:
                case Node::ST_EOL:
                    throw std::runtime_error("no operation is allowed outside functions");
                case Node::ST_FROM:
                    throw std::runtime_error("unexpected import");
                case Node::ST_NONE:
                default:
                    throw std::runtime_error("broken sToken");
            }
            it = this->parser.holder.erase(it);
            --end;
        }
    }

    void MakePackage::doMakeClassBodies() {
        for(auto &el : this->package->elements){
            switch(el.kind){
                case Package::Element::CLASS:
                    ((Class*)el.pointer)->makeBody(this);
                    break;
                case Package::Element::FUN:
                case Package::Element::VARIABLE:
                case Package::Element::PACKAGE:
                case Package::Element::NONE:
                    break;
            }
        }
    }

    void MakePackage::doComplete() {
        for(auto &el : this->package->elements){
            switch(el.kind){
                case Package::Element::CLASS:
                    ((Class*)el.pointer)->completeBody(this);
                    break;
                case Package::Element::FUN:
                    ((Fun *) el.pointer)->completeBody(this);
                    break;
                case Package::Element::VARIABLE:
                case Package::Element::PACKAGE:
                case Package::Element::NONE:
                    break;
            }
        }
    }

    Package *MakePackage::doPackage(Node &node, std::string fileName) {
        auto it = node.operands.begin();
        Package *pack;
        if(it->token == Node::T_ID){
            pack = rootPackage.pushPackage(std::move(it->str));
        }else{
            pack = doDotPackage(*it,&rootPackage);
        }

        pack = pack->pushPackage(std::move(fileName));
        node.operands.clear();
        return pack;
    }

    Package *MakePackage::doDotPackage(Node &dot, Package *father) {
        auto it = dot.operands.begin();
        father = father->pushPackage(std::move(it->str));
        ++it;
        if(it->token == Node::T_ID){
            return father->pushPackage(std::move(it->str));
        }else{
            return doDotPackage(*it,father);
        }
    }

    void MakePackage::doClass(Node &node, Package &pack) {
        Class clas(node,this,&pack);
        if(isCompleteGlobalElementExist(&clas,Package::Element::CLASS)){
            MULTIPLE_DEF:
            throw std::runtime_error("multiple definition of class "+clas.name);
        }
        auto elp = pack.find(&clas,Package::Element::CLASS);
        auto p = (Class*)(elp!=nullptr?elp->pointer:nullptr);
        if(p == nullptr){
            p = (Class*)pack.push(new Class(std::move(clas)),Package::Element::CLASS,true);
        }else if(p->isIncomplete()){
            *p = std::move(clas);
        }else{
            goto MULTIPLE_DEF;
        }
        globalElements.push(p,Package::Element::CLASS,false);
    }

    void MakePackage::doFun(Node &node, Package &pack) {
        Fun fun(node,this,Parent(&pack));
        if(isCompleteGlobalElementExist(&fun,Package::Element::FUN)){
            MULTIPLE_DEF:
            throw std::runtime_error("multiple definition of fun "+fun.name);
        }
        auto elp = pack.find(&fun,Package::Element::CLASS);
        auto p = (Fun*)(elp!=nullptr?elp->pointer:nullptr);
        if(p == nullptr){
            p = (Fun*)pack.push(new Fun(std::move(fun)),Package::Element::FUN,true);
        }else if(p->isIncomplete()){
            *p = std::move(fun);
        }else{
            goto MULTIPLE_DEF;
        }
        globalElements.push(p,Package::Element::FUN,false);
    }

    void MakePackage::doVariable(Node &node, Package &pack) {
        Variable var{node,this,Parent(&pack)};
        if(isCompleteGlobalElementExist(&var,Package::Element::VARIABLE)){
            MULTIPLE_DEF:
            throw std::runtime_error("multiple definition of variable "+var.name);
        }
        auto elp = pack.find(&var,Package::Element::CLASS);
        auto p = (Variable*)(elp!=nullptr?elp->pointer:nullptr);
        if(p == nullptr){
            p = (Variable*)pack.push(new Variable(std::move(var)),Package::Element::VARIABLE,true);
        }else if(p->isIncomplete()){
            *p = std::move(var);
        }else{
            goto MULTIPLE_DEF;
        }
        globalElements.push(p,Package::Element::VARIABLE,false);
    }

    void MakePackage::doImports() {
        auto it = this->parser.holder.begin();
        for(auto end = this->parser.holder.end();it < end && it->sToken == Node::ST_FROM;++it){
            doImport(*it);
        }
        this->parser.holder.erase(
                this->parser.holder.begin(),
                it
            );
    }

    void MakePackage::doImport(Node &node) {
        auto p = &rootPackage;
        auto it = node.operands.begin();
        //from
        if(it->token == Node::T_ID){
            p = p->pushPackage(std::move(it->str));
        }else{
            p = doDotPackage(*it,p);
        }
        ++it;
        //import
        auto end = node.operands.end();
        auto &makeFile = cf->getMakeFile(cf->getFilePath(p));
        if(it+1 == end && it->str == "*"){
            for(const auto &el : makeFile.package->elements) {
                globalElements.push(el.pointer,el.kind,false);
            }
            ++it;
        }else{
            for (; it < end; ++it) {
                auto elP = makeFile.findGlobalID(it->str);
                globalElements.push(elP->pointer,elP->kind,false);
            }
        }
    }

    Package::Element *MakePackage::findGlobal(void *pointer, Package::Element::Kind kind) {
        return globalElements.find(pointer,kind);
    }

    void *MakePackage::findGlobal(std::string_view str, Package::Element::Kind kind) {
        return globalElements.find(str,kind);
    }

    Package::Element *MakePackage::findGlobalID(std::string_view str) {
        return globalElements.findAny(str);
    }

    bool MakePackage::isCompleteGlobalIDExist(std::string_view str) {
        auto p = findGlobalID(str);
        return p != nullptr && !p->isIncomplete();
    }

    bool MakePackage::isCompleteGlobalElementExist(void *pointer, Package::Element::Kind kind) {
        auto p = findGlobal(pointer,kind);
        return p != nullptr && !p->isIncomplete();
    }
} // felan
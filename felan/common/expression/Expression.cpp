//
// Created by a404m on 9/19/22.
//

#include "Expression.h"
#include <felan/compiler/make_package/MakePackage.h>
#include <felan/common/class/Class.h>
#include <algorithm>

namespace felan {
    const Expression::OpToStr Expression::opToStr{
        //{Node::OP_DOT,""},
        //{Node::OP_FUN_CALLER,""},
        {Node::OP_POSITIVE,"__positive__"},
        {Node::OP_NEGATIVE,"__negative__"},
        {Node::OP_PLUS,"__plus__"},
        {Node::OP_MINUS,"__minus__"},
        {Node::OP_ASSIGN,"__assign__"},
        //{Node::OP_COMMA,""}
    };

    Expression::Operand::Operand(void *_pointer, Expression::Operand::Kind _kind,bool _owns)
    : pointer(_pointer), kind(_kind), owns(_owns){
        //empty
    }

    Expression::Operand::~Operand() {
        if(owns){
            switch(this->kind){
                case INT:
                case STRING:
                    delete (std::string *)this->pointer;
                    break;
                case EXPRESSION:
                    delete (Expression*)this->pointer;
                    break;
                case VARIABLE:
                    delete (Variable*)this->pointer;
                    break;
                case NONE:
                default:
                    throw std::runtime_error("broken operand");
            }
        }
    }

    Expression::Operand::Operand(Expression::Operand &&op) noexcept
    : pointer(op.pointer), kind(op.kind), owns(op.owns){
        op.owns = false;
    }

    Expression::Operand &Expression::Operand::operator=(Expression::Operand &&op)  noexcept {
        this->pointer = op.pointer;
        this->kind = op.kind;
        this->owns = op.owns;
        op.owns = false;
        return *this;
    }

    Class *Expression::Operand::getType(MakePackage *mp) const {
        Class *type;
        switch(this->kind){
            case INT:
                type = mp->findClass("Int");
                break;
            case STRING:
                type = mp->findClass("String");
                break;
            case EXPRESSION:
                type = ((Expression*)this->pointer)->fun->retType;
                break;
            case VARIABLE:
                type = ((Variable*)this->pointer)->type;
                break;
            case NONE:
            default:
                throw std::runtime_error("broken kind");
        }
        if(type == nullptr){
            throw NotFoundError("Type not found");
        }
        return type;
    }

    Expression::Expression(Node &node, MakePackage *mp, Fun *parentFun) :
            fun(nullptr), operands(){
        std::string funName;
        auto it = node.operands.begin();
        std::vector<Class*> arguments{};

        if(node.sToken == Node::ST_VAR){
            goto DO_VAR;
        }else if(node.sToken == Node::ST_FUN_CALL){
            ++it;
        }

        for(auto end = node.operands.end();it < end;++it){
            auto &n = *it;
            switch(n.token){
                case Node::T_STR:
                    this->operands.emplace_back(new std::string(n.str),Operand::STRING,true);
                    break;
                case Node::T_NUM:
                    this->operands.emplace_back(new std::string(n.str),Operand::INT,true);
                    break;
                case Node::T_ID:{
                    auto varP = parentFun->findVar(n.str);
                    if(varP){
                        this->operands.emplace_back(varP,Operand::VARIABLE,false);
                    }else{
                        auto elP = mp->findGlobalID(n.str);
                        if(elP){
                            PUSH_ELP:
                            if(elP->kind != Package::Element::VARIABLE){
                                throw std::runtime_error(std::string(elP->getName())+" is not an operand");
                            }else{
                                this->operands.emplace_back(elP->pointer,Operand::VARIABLE,false);
                            }
                        }else{
                            elP = MakePackage::rootPackage.findAny(n.str);
                            if (elP) {
                                goto PUSH_ELP;
                            } else {
                                throw std::runtime_error(n.str + " id not found");
                            }
                        }
                    }
                }break;
                case Node::T_OP: {
                    if(n.equals(Node::OP_DOT)) {
                        auto elP = doDot(n, mp, parentFun);
                        switch (elP->kind) {
                            case Package::Element::VARIABLE:
                                this->operands.emplace_back(elP->pointer, Operand::VARIABLE,false);
                                arguments.emplace_back(((Variable*)elP->pointer)->type);
                                break;
                            case Package::Element::PACKAGE:
                            case Package::Element::CLASS:
                            case Package::Element::FUN:
                                throw std::runtime_error(std::string(elP->getName()) + "is not a operand");
                            case Package::Element::NONE:
                            default:
                                throw std::runtime_error("unexpected kind");
                        }
                    }else{
                        this->operands.emplace_back(new Expression(n,mp,parentFun),Operand::EXPRESSION,true);
                    }
                }break;
                case Node::T_KW: {
                    ///@warning the only allowed keyword is var defining
                    //todo change later
                    auto varName = n.operands.front().str;
                    Expression(n, mp, parentFun);
                    this->operands.emplace_back(parentFun->findVar(varName), Operand::VARIABLE,false);
                }break;
                case Node::T_SML:
                case Node::T_NONE:
                default:
                    throw std::runtime_error("unexpected token");
            }
            arguments.emplace_back(this->operands.back().getType(mp));
        }

        switch(node.sToken){
            case Node::ST_LEFT:
            case Node::ST_RIGHT:
            case Node::ST_BOTH:
                funName = opToStr.at(node);
                switch(this->operands.front().kind){
                    case Operand::INT:
                    case Operand::STRING:
                        case Operand::EXPRESSION:{
                        auto clas = operands.front().getType(mp);
                        arguments.erase(arguments.begin());
                        this->fun = clas->getMethod(funName,arguments);
                    }break;
                    case Operand::VARIABLE:{
                        auto varP = (Variable*)this->operands.front().pointer;
                        auto clas = varP->type;
                        if(varP->isIncomplete()){
                            throw std::runtime_error("usage of incomplete variable" + varP->name);
                        }
                        arguments.erase(arguments.begin());
                        this->fun = clas->getMethod(funName,arguments);
                    }break;
                    case Operand::NONE:
                    default:
                        throw std::runtime_error("unexpected kind");
                }
                if(this->fun == nullptr){
                    if(funName == "__assign__" &&
                            this->operands.front().getType(mp) == this->operands.back().getType(mp) &&
                            this->operands.front().kind == Operand::VARIABLE){
                        this->fun = &Fun::assign;
                    }else{
                        throw std::runtime_error("no fun found");
                    }
                }
                break;
            case Node::ST_FUN_CALL:
                if(node.operands.front().equals(Node::OP_DOT)){
                    //todo delete these
                    auto elP = doDot(node.operands.front(),mp,parentFun);
                    if(elP->kind != Package::Element::FUN){
                        throw std::runtime_error("no function found");
                    }
                    auto tempFunP = (Fun*)elP->pointer;
                    auto funP = Fun(tempFunP->name);
                    funP.arguments = arguments;
                    if(funP == *tempFunP){
                        fun = (Fun*)elP->pointer;
                    }else{
                        auto fParent = tempFunP->parent;
                        switch(fParent.kind){//todo
                            case Parent::NONE:
                                break;
                            case Parent::PACKAGE:
                                break;
                            case Parent::CLASS:
                                break;
                            case Parent::FUN:
                                break;
                        }
                    }
                }else{
                    funName = node.operands.front().str;
                    if(parentFun->parent.kind == Parent::CLASS){
                        auto pfClass = (Class*)parentFun->parent.pointer;
                        auto elP = pfClass->findAny(node.operands.front().str);
                        if(elP == nullptr || elP->kind != Package::Element::FUN){
                            fun = mp->findFun(funName, arguments);
                        }else{
                            fun = (Fun*)elP->pointer;
                        }
                    }else{
                        fun = mp->findFun(funName, arguments);
                    }
                }
                break;
            case Node::ST_VAR:
                DO_VAR:
                parentFun->addVar(node,mp);
                return;
            case Node::ST_NONE:
            case Node::ST_EOL:
            case Node::ST_CLASS:
            case Node::ST_FUN:
            case Node::ST_FROM:
            case Node::ST_BETWEEN:
            default:
                throw std::runtime_error("unexpected keyword or symbol " + node.str);
        }


    }

    void Expression::doVarOperand(std::string_view varName, MakePackage *mp, Fun *parentFun) {
        auto *var = parentFun->findVar(varName);
        if(var == nullptr){
            var = mp->findVar(varName);
        }
        operands.emplace_back(var,Operand::VARIABLE,false);
    }

    Package::Element *Expression::doDot(Node &n, MakePackage *mp, Fun *parentFun) {
        std::string_view elName = n.operands.front().str;
        Variable *varP = nullptr;
        if(parentFun != nullptr)
            varP = parentFun->findVar(elName);
        Package::Element *elP;
        Package::Element *elPFirst = nullptr;
        if (!varP) {
            elP = mp->findGlobalID(elName);
            if (!elP) {
                elP = MakePackage::rootPackage.findAny(elName);
                if(!elP){
                    throw std::runtime_error("no id found "+std::string(elName));
                }
            }
        } else {
            elP = new Package::Element(varP, Package::Element::VARIABLE, false);
            elPFirst = elP;
        }
        auto itNode = n.operands.end() - 1;
        while (true) {
            bool isDone = false;
            if (itNode->equals(Node::OP_DOT)) {
                itNode = itNode->operands.begin();
            } else {
                isDone = true;
            }
            elName = itNode->str;
            switch (elP->kind) {
                case Package::Element::CLASS:
                    elP = ((Class *) elP->pointer)->findAny(elName);
                    break;
                case Package::Element::PACKAGE:
                    elP = ((Package *) elP->pointer)->findAny(elName);
                    break;
                case Package::Element::VARIABLE:
                    elP = ((Variable *) elP->pointer)->type->findAny(elName);
                    break;
                case Package::Element::FUN:
                    throw std::runtime_error("no " + std::string(elName) + " found");
                case Package::Element::NONE:
                default:
                    throw std::runtime_error("broken element");
            }
            if (isDone)
                break;
            ++itNode;
        }
        delete elPFirst;
        return elP;
    }

    std::pair<std::string, std::string> Expression::dotToString(Node &n) {
        std::string path;
        Node *np = &n;
        do{
            path += np->operands.front().str;
            np = &np->operands.back();
        }while(np->equals(Node::OP_DOT));
        return {path,np->str};
    }
} // felan
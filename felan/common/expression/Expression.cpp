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
    std::vector<std::string> Expression::values{};

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

    Class *Expression::Operand::getType() {
        switch(this->kind){
            case INT:
                return (Class*)MakePackage::rootPackage
                .getByPath("felan.lang.primitive.Int",
                           Package::Element::CLASS)->pointer;
            case STRING:
                return (Class*)MakePackage::rootPackage
                .getByPath("felan.lang.string.String",
                           Package::Element::CLASS)->pointer;
            case EXPRESSION:
                return ((Expression*)this->pointer)->fun->retType;
            case VARIABLE:
                return ((Variable*)this->pointer)->type;
            case NONE:
            default:
                throw std::runtime_error("broken kind");
        }
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
                    arguments.emplace_back((Class*)mp->findGlobal("String",Package::Element::CLASS));
                    if(arguments.back() == nullptr){
                        throw std::runtime_error("String class not found");
                    }
                    this->operands.emplace_back(new std::string(n.str),Operand::STRING,true);
                    break;
                case Node::T_NUM:
                    arguments.emplace_back((Class*)mp->findGlobal("Int",Package::Element::CLASS));
                    if(arguments.back() == nullptr){
                        throw std::runtime_error("Int class not found");
                    }
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
                    arguments.emplace_back(((Variable*)this->operands.back().pointer)->type);
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
                        arguments.emplace_back(((Expression*)this->operands.back().pointer)->fun->retType);
                        if(arguments.back() == nullptr){
                            throw std::runtime_error("argument function returns nothing");
                        }
                    }
                }break;
                case Node::T_KW: {
                    ///@warning the only allowed keyword is var defining
                    //todo change later
                    auto varName = n.operands.front().str;
                    Expression(n, mp, parentFun);
                    this->operands.emplace_back(parentFun->findVar(varName), Operand::VARIABLE,false);
                    arguments.emplace_back(((Variable *) this->operands.back().pointer)->type);
                }break;
                case Node::T_SML:
                case Node::T_NONE:
                default:
                    throw std::runtime_error("unexpected token");
            }
        }

        switch(node.sToken){
            case Node::ST_LEFT:
            case Node::ST_RIGHT:
            case Node::ST_BOTH:
                funName = opToStr.at(node);
                switch(this->operands.front().kind){
                    case Operand::INT:{
                        auto clas = (Class*)mp->findGlobal("Int",Package::Element::CLASS);
                        if(clas == nullptr){
                            throw std::runtime_error("class Int not found");
                        }
                        arguments.erase(arguments.begin());
                        this->fun = clas->getMethod(funName,arguments);
                    }break;
                    case Operand::STRING:{
                        auto clas = (Class*)mp->findGlobal("String",Package::Element::CLASS);
                        if(clas == nullptr){
                            throw std::runtime_error("class String not found");
                        }
                        arguments.erase(arguments.begin());
                        this->fun = clas->getMethod(funName,arguments);
                    }break;
                    case Operand::EXPRESSION:{
                        auto clas = (Class*)this->operands.front().pointer;
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
                    this->operands.front().getType()
                    == this->operands.back().getType() &&
                    this->operands.front().kind == Operand::VARIABLE){
                        this->fun = &Fun::assign;
                    }else{
                        throw std::runtime_error("no fun found");
                    }
                }
                break;
            case Node::ST_FUN_CALL:
                if(node.operands.front().equals(Node::OP_DOT)){
                    auto elP = doDot(node.operands.front(),mp,parentFun);
                    if(elP->kind != Package::Element::FUN){
                        throw std::runtime_error("no function found");
                    }
                    fun = (Fun*)elP->pointer;
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
} // felan
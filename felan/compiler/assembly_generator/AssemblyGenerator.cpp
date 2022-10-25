//
// Created by a404m on 10/14/22.
//

#include "AssemblyGenerator.h"
#include <felan/common/fun/Fun.h>
#include <felan/common/class/Class.h>

namespace felan {
    AssemblyGenerator::AssemblyGenerator(Fun *_mainFun) : mainFun(_mainFun) {
        //empty
    }

    std::string AssemblyGenerator::compile() {
        compileFun(mainFun);
        std::string code;
        code += ".data\n";
        for(auto &pair : constValues){
            code += pair.first;
            code += ' ';
            code += pair.second;
            code += '\n';
        }
        code += ".code\n";
        code += callFun(mainFun);
        code += "\nEXIT\n";
        for(auto &function : functions){
            code += function;
            code += '\n';
        }
        return code;
    }

    std::string AssemblyGenerator::callFun(Fun *fun) {
        if(fun->attribute.has(Attribute::ATT_NATIVE)){
            if(fun->parent.kind != Parent::CLASS){
                WANTS_NATIVE_CLASS:
                throw SyntaxError("native functions are inside native classes");
            }
            auto clas = (Class*)fun->parent.pointer;
            if(!clas->attribute.has(Attribute::ATT_NATIVE)
            ||  clas->attribute.native.empty()){
                goto WANTS_NATIVE_CLASS;
            }
            std::string key = "native."+getName(clas);
            addConst(key,'\"'+clas->attribute.native+'\"');
            std::string key2 = "builtin."+clas->attribute.native;
            addConst(key2,getName(fun));
            std::string key3 = "sign."+key2;
            addConst(key3,getSign(fun));
            return
            "SPUSH "+key3+"\n"+
            "SPUSH "+key2+"\n"+
            "SPUSH "+key+"\n"+
            "CALL_NATIVE ";
        }else if(fun->attribute.has(Attribute::ATT_BUILTIN)) {
            return "CALL_BUILTIN "+getName(fun);
        }else{
            if(!names.contains(fun)){
                compileFun(fun);
            }
            return "CALL #"+getName(fun);
        }
    }

    void AssemblyGenerator::compileFun(Fun *fun) {
        if(names.contains(fun)){
            return;
        }
        std::string code;
        code += '#';
        code += getName(fun);
        code += '\n';
        code += "PUSH "+std::to_string(fun->vars.size());
        code += "\nALLOC_NVAR\n";
        for(int64_t i = fun->paramSize-1;i >= 0;--i){
            code += "OPOP "+ std::to_string(i);
            code += '\n';
        }
        for(auto &expr : fun->body){
            code += compileExpression(&expr);
            code += '\n';
        }
        code += "RET";
        functions.emplace_back(code);
        names.emplace(fun,getName(fun));
    }

    std::string AssemblyGenerator::compileExpression(Expression *expr) {
        std::string code;
        if(expr->fun == &Fun::assign){
            code += pushOperand(expr->operands.back());
            code += '\n';
            code += popVar((Variable*)expr->operands.front().pointer);
            return code;
        }
        for(auto it = expr->operands.begin(),end = expr->operands.end();it < end;++it){
            code += pushOperand(*it);
            code += '\n';
        }
        code += callFun(expr->fun);

        return code;
    }

    std::string AssemblyGenerator::pushVar(Variable *var) {
        std::string code;
        if(var->parent.kind == Parent::FUN){
            code = "OPUSH ";
        }else{
            code = "SPUSH ";
        }
        return code+getName(var);
    }

    std::string AssemblyGenerator::pushOperand(Expression::Operand &operand) {
        switch(operand.kind){
            case Expression::Operand::INT:
                addConst(
                        "%"+*(std::string*)operand.pointer,
                        *(std::string*)operand.pointer
                );
                return "SPUSH %"+*(std::string*)operand.pointer;
            case Expression::Operand::STRING:
                addConst(
                        "%%"+*(std::string*)operand.pointer,
                        '\"'+*(std::string*)operand.pointer+'\"'
                );
                return "SPUSH %%"+*(std::string*)operand.pointer;
            case Expression::Operand::VARIABLE:
                return pushVar((Variable*)operand.pointer);
            case Expression::Operand::EXPRESSION:
                return compileExpression((Expression*)operand.pointer);
            case Expression::Operand::NONE:
            default:
                throw std::runtime_error("broken expr");
        }
    }

    std::string AssemblyGenerator::popVar(Variable *var) {
        std::string code;
        if(var->parent.kind == Parent::FUN){
            code = "OPOP ";
        }else{
            //code = "SPOP ";
            throw std::runtime_error("you can't assign to static value");
        }
        return code+getName(var);
    }

    std::string AssemblyGenerator::getName(Parent parent) {
        if(parent.kind == Parent::NONE){
            return "";
        }
        std::string result;
        do{
            result = "." + parent.getName() + result;
        }while((parent = parent.getParent()).kind != Parent::NONE
                && parent.pointer != &MakePackage::rootPackage);
        if(result.front() == '.'){
            result.erase(0,1);
        }

        return result;
    }

    std::string AssemblyGenerator::getName(Class *clas) {
        std::string result = getName(clas->father) + '.' + clas->name;
        if(result.front() == '.'){
            result.erase(0,1);
        }
        return result;
    }

    std::string AssemblyGenerator::getName(Fun *fun) {
        std::string result = getName(fun->parent) + '.' + fun->name;
        if(result.front() == '.'){
            result.erase(0,1);
        }
        result += '(';
        for(const auto &arg : fun->arguments){
            result += getName(arg) + ',';
        }
        if(result.back() == '(') {
            result += ' ';
        }
        result.back() = ')';
        if(fun->retType != nullptr && fun->name != "__constructor__"){
            result += ':';
            result += getName(fun->retType);
        }
        return result;
    }

    std::string AssemblyGenerator::getName(Variable *var) {
        std::string result;
        if(var->parent.kind != Parent::FUN) {
            result = getName(var->parent) + '.' + var->name;
            if (result.front() == '.') {
                result.erase(0, 1);
            }
        }else{
            auto fun = (Fun*)var->parent.pointer;
            auto id = fun->getVarID(var);
            result = std::to_string(id);
        }
        return result;

    }

    void AssemblyGenerator::addConst(std::string key, std::string value) {
        auto it = constValues.find(key);
        if(it == constValues.end()){
            constValues.emplace(key,value);
        }
    }

    std::string AssemblyGenerator::getSign(Fun *fun) {
        static Class *Int =
                (Class*) MakePackage::rootPackage
                        .getClassOrPackageByPath(
                                "felan.lang.primitive.Int",
                                Package::Element::CLASS
                        );
        static Class *String =
                (Class*) MakePackage::rootPackage
                        .getClassOrPackageByPath(
                                "felan.lang.string.String",
                                Package::Element::CLASS
                        );

        std::string sign;
        for(const auto &clas : fun->arguments){
            if(clas == Int){
                sign += 'i';
            }else if(clas == String){
                sign += 's';
            }else{
                sign += 'o';
            }
        }
        sign += ':';
        if(fun->retType == nullptr){
            sign += 'v';
        }else if(fun->retType == Int){
            sign += 'i';
        }else if(fun->retType == String){
            sign += 's';
        }else{
            sign += 'o';
        }
        return sign;
    }
} // felan
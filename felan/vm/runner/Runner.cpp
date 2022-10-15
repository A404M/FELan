//
// Created by a404m on 9/24/22.
//

#include "Runner.h"
#include <felan/assembler/builtin_functions/BuiltinFunctions.h>
#include <chrono>
#include <iostream>
#include <algorithm>

namespace felan {
    Runner::Runner(Runner::BytecodeHolder _bytecodeHolder) :
    bytecodeHolder(std::move(_bytecodeHolder)),
    variablePointer((VariablePointer)&bytecodeHolder[8*2]),
    codePointer(static_cast<CodePointer>((void*)&bytecodeHolder[0]) + derefAs<uint64_t>((void *) &bytecodeHolder[0])),
    dataPointer(static_cast<DataPointer>((void*)&bytecodeHolder[0]) + derefAs<uint64_t>((void *) &bytecodeHolder[8])),
    env(this)
    {
        //empty
    }

    Runner::~Runner() {
        for(auto p : rawObjectPointerHolder){
            delete[] p.first;
        }
    }

    void Runner::run() {
        CodePointer cp = codePointer,
        codeEnd = (CodePointer)dataPointer;
        funVariablesStack.clear();
        stack.clear();
        callStack.clear();
        funVariablesStack.emplace();
        rawObjectPointerHolder.clear();
        rawObjectPointerHolder.reserve(1024*1024);

        while(cp < codeEnd){
            //auto start = std::chrono::high_resolution_clock::now();
            auto token = Instruction::Token(*cp);
            switch(token){
                case Instruction::EXIT:
                    EXIT:
                    return;
                case Instruction::PUSH:
                    stack.push(derefAs<uint64_t>(cp + 1));
                    cp+=sizeof(uint64_t);
                    break;
                case Instruction::OPUSH:
                    stack.push((uint64_t)funVariablesStack.top().at(derefAs<uint64_t>(cp + 1)));
                    cp+=sizeof(uint64_t);
                    break;
                case Instruction::SPUSH:
                    stack.push((uint64_t)(
                            dataPointer+
                            variablePointer[
                                derefAs<uint64_t>(cp + 1)
                                ]
                            )
                        );
                    cp += sizeof(uint64_t);
                    break;
                case Instruction::DPUSH:
                    stack.push(stack.top());
                    break;
                case Instruction::OPOP:
                    funVariablesStack.top().at(derefAs<uint64_t>(cp + 1)) = (void*)stack.getPop();
                    cp+=sizeof(uint64_t);
                    break;
                case Instruction::POPN:
                    stack.pop(stack.top()+1);
                    break;
                case Instruction::CALL:
                    funVariablesStack.emplace();
                    callStack.push((uint64_t)cp+9);
                    goto INSTRUCTION_JMP;
                    //break;
                case Instruction::CALL_BUILTIN:
                    (BuiltinFunctions::functionHolder[derefAs<uint64_t>(cp+1)])(env);
                    cp += sizeof(uint64_t);
                    break;
                case Instruction::CALL_NATIVE:
                    this->callNative();
                    break;
                case Instruction::ALLOC_NVAR:
                    funVariablesStack.top().resize(funVariablesStack.top().size()+stack.getPop());
                    break;
                case Instruction::ALLOC_NBYTE_HEAP:
                    this->makeNewObject(stack.getPop());
                    break;
                case Instruction::RET:
                    if(funVariablesStack.holder.size() == 1){
                        goto EXIT;
                    }
                    cp = (DataPointer)(callStack.getPop() - 1);
                    funVariablesStack.pop();
                    goto JMP;
                    //break;
                case Instruction::JMP:
                    INSTRUCTION_JMP:
                    cp += derefAs<int64_t>(cp + 1) + sizeof(int64_t);
                    JMP:
                    if(!isInRange(codePointer,codeEnd,cp+1)){
                        throw std::runtime_error("bad jump");
                    }
                    break;
                case Instruction::JMP_EZ:
                    if((int64_t)stack.getPop() == 0) {
                        goto INSTRUCTION_JMP;
                    }else{
                        IN_ELSE_JMPS:
                        cp += sizeof(uint64_t);
                    }
                    break;
                case Instruction::JMP_NEZ:
                    if((int64_t)stack.getPop() != 0) {
                        goto INSTRUCTION_JMP;
                    }else{
                        goto IN_ELSE_JMPS;
                    }
                    //break;
                case Instruction::JMP_GZ:
                    if((int64_t)stack.getPop() > 0) {
                        goto INSTRUCTION_JMP;
                    }else{
                        goto IN_ELSE_JMPS;
                    }
                    //break;
                case Instruction::JMP_LZ:
                    if((int64_t)stack.getPop() < 0) {
                        goto INSTRUCTION_JMP;
                    }else{
                        goto IN_ELSE_JMPS;
                    }
                    //break;
                case Instruction::JMP_GEZ:
                    if((int64_t)stack.getPop() >= 0) {
                        goto INSTRUCTION_JMP;
                    }else{
                        goto IN_ELSE_JMPS;
                    }
                    //break;
                case Instruction::JMP_LEZ:
                    if((int64_t)stack.getPop() <= 0) {
                        goto INSTRUCTION_JMP;
                    }else{
                        goto IN_ELSE_JMPS;
                    }
                    //break;
                default:
                    throw std::runtime_error("segment fault :)");
            }
            ++cp;
            //auto end = std::chrono::high_resolution_clock::now();
            //auto diff = std::chrono::duration<double,std::micro>(end-start).count();
            //std::cout << "####" << Instruction::tokenToString(token) << "\t\t" << diff << "us" << std::endl;
        }
        throw std::runtime_error("program bad exit");
    }

    void Runner::callNative() {
        auto pFileName = (felan_lang_string_String*)stack.getPop();
        auto pFunName = (felan_lang_string_String*)stack.getPop();
        auto pFunSign = (felan_lang_string_String*)stack.getPop();

        auto it = nativeLibraryLoaderHolder.find(pFileName->toString());
        NativeLibraryLoader *nll;
        if(it == nativeLibraryLoaderHolder.end()){
            nll = &(nativeLibraryLoaderHolder.emplace(
                    pFileName->toString(),pFileName->toString()
            ).first->second);
        }else{
            nll = &it->second;
        }
        nll->callFunction(pFunName->toString(),pFunSign->toStringView(),env);
    }

    template<typename T>
    T &Runner::derefAs(void *p) {
        return *static_cast<T*>(p);
    }

    template<typename T>
    bool Runner::isInRange(T begin, T end, T v) {
        return v >= begin && v < end;
    }

    void *Runner::makeNewObject(uint64_t size) {
        return rawObjectPointerHolder.emplace_back(new uint8_t[size],size).first;
    }

    void Runner::garbageCollector() {
        std::vector<bool> hasPointers(this->rawObjectPointerHolder.size());

        for(auto it = this->rawObjectPointerHolder.begin(),end = this->rawObjectPointerHolder.end();it < end;++it){
            auto pointer = it->first;
            if(this->hasInStack((uint64_t)pointer))
                goto HAS_POINTER;

            if(this->hasInVariable(pointer))
                goto HAS_POINTER;

            hasPointers[it-this->rawObjectPointerHolder.begin()] = false;
            continue;
            HAS_POINTER:
            hasPointers[it-this->rawObjectPointerHolder.begin()] = true;
        }

        BEGIN:
        bool changed = false;

        for(uint64_t i = 0;i < this->rawObjectPointerHolder.size();++i){
            if(!hasPointers[i]){
                auto p = this->rawObjectPointerHolder[i].first;

                for(uint64_t j = 0;j < this->rawObjectPointerHolder.size();++j){
                    if(hasPointers[j]){
                        auto pointer = this->rawObjectPointerHolder[j].first;
                        auto size = this->rawObjectPointerHolder[j].second;
                        if(hasPointerTo(pointer,pointer+size,p)){
                            hasPointers[i] = changed = true;
                        }
                    }
                }
            }
        }
        if(changed){
            goto BEGIN;
        }

        auto eEnd = (int64_t)(this->rawObjectPointerHolder.size()-1);
        for(auto i = eEnd;i > -1;--i){
            if(hasPointers[i]){
                this->rawObjectPointerHolder.erase(
                        this->rawObjectPointerHolder.begin()+i,
                        this->rawObjectPointerHolder.begin()+eEnd
                    );
                eEnd = i-1;
            }
        }
        this->rawObjectPointerHolder.erase(
                this->rawObjectPointerHolder.begin(),
                this->rawObjectPointerHolder.begin()+eEnd
        );
    }

    bool Runner::hasInStack(uint64_t value) {
        return std::find(this->stack.holder.begin(),this->stack.holder.end(),value)
                != this->stack.holder.end();
    }

    bool Runner::hasInVariable(Runner::Object object) {
        return std::find_if(this->funVariablesStack.holder.begin(), this->funVariablesStack.holder.end(),
                         [object](const VariableHolder &variableHolder)->bool{
            return std::find(variableHolder.begin(), variableHolder.end(),object)
                    != variableHolder.end();
        }) != this->funVariablesStack.holder.end();
    }

    template<typename T>
    bool Runner::hasPointerTo(uint8_t *begin, const uint8_t *end, T t) {
        auto p = begin;
        end -= sizeof(T)-1;
        while(p < end){
            if(derefAs<T>(p) == t){
                return true;
            }
            ++p;
        }
        return false;
    }

} // felan
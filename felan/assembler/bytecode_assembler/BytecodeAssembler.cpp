//
// Created by a404m on 9/22/22.
//

#include "BytecodeAssembler.h"
#include <algorithm>
#include <felan/assembler/builtin_functions/BuiltinFunctions.h>

namespace felan {

    BytecodeAssembler::BytecodeAssembler(const AssemblerLexer::Holder &lHolder) {
        enum Section{
            NONE,
            CODE,
            DATA
        }section = NONE;
        for(const auto &line : lHolder){
            if(line.size() == 1){
                if(line.front() == ".code"){
                    section = CODE;
                    continue;
                }else if(line.front() == ".data"){
                    section = DATA;
                    continue;
                }
            }
            switch(section){
                case CODE:
                    parseCode(line);
                    break;
                case DATA:
                    parseData(line);
                    break;
                case NONE:
                default:
                    throw std::runtime_error("bad section");
            }
        }
    }

    /*
     * the bytecode is like:
     * [code_pointer][constexpr_pointer][static_variable][code][constexpr]
     */
    BytecodeAssembler::BytecodeHolder BytecodeAssembler::compile() {
        BytecodeHolder result(8*2);

        for(const auto &el : dataIndexHolder){
            auto temp = Instruction::uint64ToStringCode(el.second);
            result.insert(
                    result.end(),
                    std::make_move_iterator(temp.begin()),
                    std::make_move_iterator(temp.end()));
        }
        uint64_t codeBeginIndex = result.size();
        {
            auto temp = Instruction::uint64ToStringCode(codeBeginIndex);
            auto it = result.begin();
            for(auto c : temp){
                *it = c;
                ++it;
            }
        }

        for(auto instruction : instructionHolder){
            if(!instruction.operand.empty()){
                if(isLabel(instruction.operand)) {
                    auto it = findInLabel(instruction.operand);
                    if (it == labelHolder.end())
                        throw std::runtime_error("undefined label " + instruction.operand);
                    instruction.operand = std::to_string(findInLabel(instruction.operand)->second - (result.size() + 9 - codeBeginIndex));
                }else if(isID(instruction.operand)){
                    auto it = findInData(instruction.operand);
                    if(it == dataIndexHolder.end())
                        throw std::runtime_error("undefined id "+instruction.operand);
                    instruction.operand = std::to_string(it - dataIndexHolder.begin());
                }
            }
            auto temp = instruction.toStringCode();
            result.insert(result.end(),
                          std::make_move_iterator(temp.begin()),
                          std::make_move_iterator(temp.end()));
        }
        {
            auto temp = Instruction::uint64ToStringCode(result.size());
            auto it = result.begin()+8;
            for(auto c : temp){
                *it = c;
                ++it;
            }
        }
        result.insert(
                result.end(),
                constexprHolder.begin(),constexprHolder.end());

        return result;
    }

    void BytecodeAssembler::parseData(const AssemblerLexer::Line &line) {
        if(line.size() != 2){
            throw std::runtime_error("data needs 2 element in a line");
        }
        const auto &name = line.front();
        if(isNumber(name) || isString(name)){
            throw std::runtime_error("data needs a name");
        }
        auto value = line.back();
        auto index = constexprHolder.size();
        if(isString(value)){
            value.erase(value.begin());//' or "
            value += '\0';
        }else if(isNumber(value)){
            value = Instruction::stringNumberToStringCode(value);
        }else{
            throw std::runtime_error("data needs a value");
        }
        constexprHolder.insert(constexprHolder.end(),value.begin(),value.end());
        dataIndexHolder.emplace_back(name,index);
    }

    void BytecodeAssembler::parseCode(const AssemblerLexer::Line &line) {
        if(line.size() == 1 && isLabel(line.front())){
            labelHolder.emplace_back(std::string{line.front().begin(), line.front().end()},codeActualSize);
            return;
        }
        Instruction::Token token = Instruction::stringToToken.at(toUpper(line.front()));
        auto &ops = Instruction::operandGuider[token];
        if(ops.size() != line.size()-1){
            throw std::runtime_error(Instruction::tokenToString(token) + "needs " + std::to_string(ops.size()) + " operand" + (ops.size()>1?"s":""));
        }
        codeActualSize += 1+ops.size()*8;
        if(line.size() == 1){
            instructionHolder.emplace_back(token);
        }else if(line.size() == 2) {
            if (isString(line.back())
                || (isNumber(line.back()) && !Instruction::has(ops.back(), Instruction::INT))
                || (isLabel(line.back()) && !Instruction::has(ops.back(), Instruction::LABEL))
                || (isID(line.back()) && !Instruction::has(ops.back(), Instruction::ID))){
                throw std::runtime_error("operand mismatch for " + Instruction::tokenToString(token));
            }else{
                auto operand = line.back();
                if(token == Instruction::CALL_BUILTIN && !isNumber(operand)){
                    operand = std::to_string(BuiltinFunctions::functionToIndex.at(operand));
                }
                instructionHolder.emplace_back(token, operand);
            }
        }else{
            throw std::runtime_error("instruction wants one or two in a line");
        }
    }

    bool BytecodeAssembler::isString(std::string_view str) {
        auto c = str.front();
        return c == '\'' || c == '\"';
    }

    bool BytecodeAssembler::isNumber(std::string_view str) {
        char c = str.front();
        return std::isdigit(c) || c == '-' || c == '+';
    }

    bool BytecodeAssembler::isLabel(std::string_view str) {
        return str.front() == '#';
    }

    bool BytecodeAssembler::isID(std::string_view str) {
        return !isString(str) && !isNumber(str) && !isLabel(str);
    }

    std::string BytecodeAssembler::toUpper(std::string str) {
        std::for_each(str.begin(),str.end(),[](char &c){c = (char)std::toupper(c);});
        return str;
    }

    BytecodeAssembler::DataIndexHolder::iterator BytecodeAssembler::findInData(std::string_view name) {
        return std::find_if(dataIndexHolder.begin(),dataIndexHolder.end(),[name](const DataIndexHolder::value_type &p)->bool{return name == p.first;});
    }

    std::vector<std::pair<std::string, uint64_t>>::iterator BytecodeAssembler::findInLabel(std::string_view name) {
        return std::find_if(labelHolder.begin(),labelHolder.end(),[name](const LabelHolder ::value_type &p)->bool{return name == p.first;});
    }

} // felan
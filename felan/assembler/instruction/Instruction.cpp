//
// Created by a404m on 9/22/22.
//

#include "Instruction.h"
#include <algorithm>

namespace felan {
    const Instruction::StringToToken Instruction::stringToToken = {
            {"EXIT",EXIT},
            {"PUSH",PUSH},
            {"OPUSH",OPUSH},
            {"SPUSH",SPUSH},
            {"DPUSH",DPUSH},
            {"OPOP",OPOP},
            {"POPN",POPN},
            {"CALL",CALL},
            {"CALL_BUILTIN",CALL_BUILTIN},
            {"CALL_NATIVE",CALL_NATIVE},
            {"ALLOC_NVAR",ALLOC_NVAR},
            {"ALLOC_NBYTE_HEAP",ALLOC_NBYTE_HEAP},
            {"RET",RET},
            {"JMP",JMP},
            {"JMP_EZ",JMP_EZ},
            {"JMP_NEZ",JMP_NEZ},
            {"JMP_GZ",JMP_GZ},
            {"JMP_LZ",JMP_LZ},
            {"JMP_GEZ",JMP_GEZ},
            {"JMP_LEZ",JMP_LEZ},
    };

    const Instruction::OperandGuider Instruction::operandGuider = {
            {},                                 //EXIT
            {INT},                              //PUSH
            {INT},                              //OPUSH
            {ID},                               //SPUSH
            {},                                 //DPUSH
            {INT},                              //OPOP
            {},                                 //POPN
            {(OperandKind)(INT|LABEL)},         //CALL
            {(OperandKind)(INT|ID)},            //CALL_BUILTIN
            {},                                 //CALL_NATIVE
            {},                                 //ALLOC_NVAR
            {},                                 //ALLOC_NBYTE_HEAP
            {},                                 //RET
            {(OperandKind)(INT|LABEL)},         //JMP
            {(OperandKind)(INT|LABEL)},         //JMP_EZ
            {(OperandKind)(INT|LABEL)},         //JMP_NEZ
            {(OperandKind)(INT|LABEL)},         //JMP_GZ
            {(OperandKind)(INT|LABEL)},         //JMP_LZ
            {(OperandKind)(INT|LABEL)},         //JMP_GEZ
            {(OperandKind)(INT|LABEL)},         //JMP_LEZ
    };

    Instruction::Instruction(Token _token) : token(_token), operand() {
        //empty
    }

    Instruction::Instruction(Token _token, std::string _operand) : token(_token), operand(std::move(_operand)) {
        //empty
    }

    std::string Instruction::toStringCode() const {
        std::string result;
        result = (char) token;
        if (operand.empty()){
            return result;
        }else{
            return result+=stringNumberToStringCode(operand);
        }
    }

    std::string Instruction::uint64ToStringCode(uint64_t v) {
        return {(char*)&v, (char*)&v + sizeof(uint64_t)};
    }

    uint64_t Instruction::hexToUInt64(std::string_view hex) {
        uint64_t result = 0;
        for(auto c : hex){
            if(c >= '0' && c <= '9'){
                result += c-'0';
            }else if(c >= 'a' && c <= 'z'){
                result += (c-'a')+10;
            }else{
                throw std::runtime_error("unknown hex value " + std::string(hex));
            }
            result <<= 4;
        }
        return result;
    }

    std::string Instruction::stringNumberToStringCode(std::string_view num) {
        uint64_t result;
        bool isNeg = false;
        if(num.front() == '+'){
            POS:
            num = std::string_view(num.begin()+1,num.end());
            goto DECIMAL;
        }else if(num.front() == '-'){
            isNeg = true;
            goto POS;
        }else{
            if(num.front() == '0'){
                if(num.size() == 1){
                    result = 0;
                }else if(num[1] == 'x'){
                    result = hexToUInt64(std::string_view(num.begin()+2, num.end()));
                }else{
                    DECIMAL:
                    result = std::stoull(std::string(num));
                }
            }else{
                goto DECIMAL;
            }
        }
        if(isNeg){
            result = -result;
        }
        return uint64ToStringCode(result);
    }

    std::string Instruction::tokenToString(Instruction::Token t) {
        auto it = std::find_if(stringToToken.begin(), stringToToken.end(),[t](decltype(stringToToken)::const_reference pair){return pair.second == t;});
        if(it == stringToToken.end()){
            throw std::runtime_error("no token found");
        }else{
            return it->first;
        }
    }

    bool Instruction::has(Instruction::OperandKind h, Instruction::OperandKind opKind) {
        return h&opKind;
    }

} // felan
//
// Created by a404m on 9/22/22.
//

#ifndef FVM_INSTRUCTION_H
#define FVM_INSTRUCTION_H

#include <string>
#include <map>
#include <vector>
#include <stdexcept>

namespace felan {

    class Instruction {
    public:
        enum Token : uint8_t {
            EXIT = 0,           //exits the program
            PUSH,               //pushes a qword int into stack
            OPUSH,              //pushes a qword from variable num into stack
            SPUSH,              //pushes a qword from static variable num
            DPUSH,              //duplicates stack top
            OPOP,               //pops a qword from stack into variable num
            POPN,               //pops n+1 (n is from stack) qword from stack without saving
            CALL,               //calls a function by its starting address
            CALL_BUILTIN,       //calls a builtin function by index
            CALL_NATIVE,        //calls a native function by its name and so file name (which they all are from stack)
            ALLOC_NVAR,         //allocates n (n is from stack) qwords in stack for function
            ALLOC_NBYTE_HEAP,   //allocates n (n is from stack) bytes in heap and returns a ref to it
            RET,                //returns cp from subroutine into caller
            JMP,                //unconditional jump n byte (if n is negative jumps backward)
            JMP_EZ,             //jumps if stack top == 0
            JMP_NEZ,            //jumps if stack top != 0
            JMP_GZ,             //jumps if stack top > 0
            JMP_LZ,             //jumps if stack top < 0
            JMP_GEZ,            //jumps if stack top >= 0
            JMP_LEZ,            //jumps if stack top <= 0
        };

        enum OperandKind : uint8_t {
            LABEL = 0b1,
            INT = 0b10,
            ID = 0b100
        };
        typedef std::map<std::string,Token> StringToToken;
        typedef std::vector<std::vector<OperandKind>> OperandGuider;

        static const StringToToken stringToToken;
        static const OperandGuider operandGuider;
        Token token;
        std::string operand;

        explicit Instruction(Token _token);
        Instruction(Token _token,std::string _operand);

        [[nodiscard]] std::string toStringCode() const;

        static std::string uint64ToStringCode(uint64_t v);
        static uint64_t hexToUInt64(std::string_view hex);
        static std::string stringNumberToStringCode(std::string_view num);

        static std::string tokenToString(Token t);

        static bool has(OperandKind h,OperandKind opKind);
    };

} // felan

#endif //FVM_INSTRUCTION_H

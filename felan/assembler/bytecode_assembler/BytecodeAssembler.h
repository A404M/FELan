//
// Created by a404m on 9/22/22.
//

#ifndef FVM_BYTECODEASSEMBLER_H
#define FVM_BYTECODEASSEMBLER_H

#include "felan/assembler/instruction/Instruction.h"
#include "felan/assembler/assembler_lexer/AssemblerLexer.h"

namespace felan {

    class BytecodeAssembler {
    public:
        typedef std::vector<Instruction> InstructionHolder;
        typedef std::vector<uint8_t> DataHolder;
        typedef std::vector<std::pair<std::string,uint64_t>> DataIndexHolder;
        typedef std::vector<uint8_t> BytecodeHolder;
        typedef std::vector<std::pair<std::string,uint64_t>> LabelHolder;
    private:
        InstructionHolder instructionHolder{};
        DataHolder constexprHolder{};
        DataIndexHolder dataIndexHolder{};
        LabelHolder labelHolder{};
        uint64_t codeActualSize = 0;

    public:
        explicit BytecodeAssembler(const AssemblerLexer::Holder &lHolder);

        BytecodeHolder compile();
    private:
        void parseData(const AssemblerLexer::Line &line);
        void parseCode(const AssemblerLexer::Line &line);

        static inline bool isString(std::string_view str);
        static inline bool isNumber(std::string_view str);
        static inline bool isLabel(std::string_view str);
        static inline bool isID(std::string_view str);

        static inline std::string toUpper(std::string str);

        DataIndexHolder::iterator findInData(std::string_view name);
        LabelHolder::iterator findInLabel(std::string_view name);
    };

} // felan

#endif //FVM_BYTECODEASSEMBLER_H

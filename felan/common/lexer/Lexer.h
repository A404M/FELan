//
// Created by a404m on 8/29/22.
//

#ifndef FELAN_LEXER_H
#define FELAN_LEXER_H

#include "felan/common/node/Node.h"

namespace felan {

    class Lexer {
        friend class Runner;
        friend class Parser;
    public:
        typedef std::vector<Node> Holder;
    private:
        Holder holder;//holds all lexed nodes

        //used for finding keywords by str
        const static std::string KW_STR_HOLDER[];
        const static std::string *KW_STR_BEGIN;
        const static std::string *KW_STR_END;
        //used for finding symbols by str
        const static std::string SML_STR_HOLDER[];
        const static std::string *SML_STR_BEGIN;
        const static std::string *SML_STR_END;
        //used with result of founded index by KW_STR_HOLDER to get sToken
        const static Node::SpecialToken KW_ST_HOLDER[];
        const static Node::SpecialToken *KW_ST_BEGIN;
        const static Node::SpecialToken *KW_ST_END;
        //used with result of founded index by SML_STR_HOLDER to get sToken
        const static Node::SpecialToken SML_ST_HOLDER[];
        const static Node::SpecialToken *SML_ST_BEGIN;
        const static Node::SpecialToken *SML_ST_END;
    public:
        explicit Lexer(std::string_view code);//the main lexing constructor
        Lexer(const Lexer &lexer) = default;//copy constructor
        Lexer(Lexer &&lexer) noexcept;//move constructor, used in vector

        std::string toString() const;//for debugging

    private:
        inline void push_clear(Node &node);//pushes the node and makes it clear
        inline void push_clear_ifn_empty(Node &node);//like last func but if node is not empty

        //used to recognize the characters
        inline static bool isNewLine(char c);
        inline static bool isSpace(char c);
        inline static bool isStringSymbol(char c);
        inline static bool isNumber(char c);
        inline static bool isLetter(char c);
        inline static bool isOperator(char c);
        inline static bool isSymbol(char c);
        inline static bool isBackSlash(char c);
        inline static bool isSlash(char c);

        ///@returns index of the keyword in @var KW_STR_HOLDER by str
        inline static long findKeyword(const std::string &str);
        ///@returns index of the symbol in @var SML_STR_HOLDER by str
        inline static long findSymbol(const std::string &str);
    };
}//namespace felan

#endif //FELAN_LEXER_H

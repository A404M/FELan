//
// Created by a404m on 8/29/22.
//

#include "Lexer.h"
#include <cmath>
#include <algorithm>
#include "felan/common/parser/Parser.h"

namespace felan {
#define FELAN_GET_ALL_KW(p)        \
    felan::Node::KW_PACKAGE.p,felan::Node::KW_IMPORT.p, \
    felan::Node::KW_FROM.p,felan::Node::KW_CLASS.p,  \
    felan::Node::KW_FUN.p,felan::Node::KW_VAR.p,        \
    felan::Node::KW_VAL.p,felan::Node::KW_RETURN.p,     \
    felan::Node::KW_PUBLIC.p,felan::Node::KW_PRIVATE.p, \
    felan::Node::KW_PROTECT.p,felan::Node::KW_OPEN.p,   \
    felan::Node::KW_CLOSE.p,felan::Node::KW_NATIVE.p,   \
    felan::Node::KW_BUILTIN.p,                           \
    felan::Node::KW_OVERRIDE.p,felan::Node::KW_OPERATOR.p,\
    felan::Node::KW_STATIC.p

#define FELAN_GET_ALL_SML(p)           \
    felan::Node::SML_NEWLINE.p,felan::Node::SML_SEMICOLON.p, \
    felan::Node::BL_CURLY_BRACKET_OPEN.p,                    \
    felan::Node::BL_CURLY_BRACKET_CLOSE.p
    const std::string Lexer::KW_STR_HOLDER[] = {
            FELAN_GET_ALL_KW(str)
    };
    const std::string *Lexer::KW_STR_BEGIN = std::begin(KW_STR_HOLDER);
    const std::string *Lexer::KW_STR_END = std::end(KW_STR_HOLDER);
    const std::string Lexer::SML_STR_HOLDER[] = {
            FELAN_GET_ALL_SML(str)
    };
    const std::string *Lexer::SML_STR_BEGIN = std::begin(SML_STR_HOLDER);
    const std::string *Lexer::SML_STR_END = std::end(SML_STR_HOLDER);
    const Node::SpecialToken Lexer::KW_ST_HOLDER[] = {
            FELAN_GET_ALL_KW(sToken)
    };
    const Node::SpecialToken *Lexer::KW_ST_BEGIN = std::begin(KW_ST_HOLDER);
    const Node::SpecialToken *Lexer::KW_ST_END = std::end(KW_ST_HOLDER);
    const Node::SpecialToken Lexer::SML_ST_HOLDER[] = {
            FELAN_GET_ALL_SML(sToken)
    };
    const Node::SpecialToken *Lexer::SML_ST_BEGIN = std::begin(SML_ST_HOLDER);
    const Node::SpecialToken *Lexer::SML_ST_END = std::end(SML_ST_HOLDER);

    Lexer::Lexer(std::string_view code) : holder() {
        Node node;//used to save str and token
        //iterates whole string
        for (auto it = code.cbegin(), end = code.cend(); it < end; ++it) {
            //current character
            char current = *it;
            //current char == '/'
            if(isSlash(current)){
                //it would be one lined comment
                if(isSlash(*(it+1))){
                    it += 2;//after "//"
                    it = std::find(it,end,'\n');//goto other line
                    push_clear_ifn_empty(node);//if the line wasn't empty before comment so push it
                    this->holder.push_back(Node::SML_NEWLINE);//it had new line because we passed it
                    continue;//no need to push current character
                }else if(*(it+1) == '*'){//multi lined comment
                    it += 3;//go two after '*' because we are looking for / and then we look if '*' is placed before it
                    bool hasNewLine = false;
                    for(;;++it){
                        if(it >= end){
                            throw std::runtime_error("no closing */");
                        }
                        if(*it == '/'){
                            if(*(it-1) == '*'){
                                break;
                            }
                        }else if(*it == '\n'){
                            hasNewLine = true;
                        }
                    }
                    if(hasNewLine){
                        push_clear_ifn_empty(node);//push everything in the line before the comment
                        this->holder.push_back(Node::SML_NEWLINE);//push the end of the line
                    }
                    continue;//don't push current character
                }
            }
            if (isBackSlash(current)) {//current == '\\'
                if (!isNewLine(*++it)) {//all \ outside of strings must be used to indicate continuing lines
                    throw std::runtime_error(std::string("unexcepted '") + current + "'");
                } else {
                    continue;//don't push new line to continue the line
                }
            } else if (isSymbol(current)) {//this condition should be before isSpace condition
                push_clear_ifn_empty(node);//symbols has length of one character
                node.token = Node::T_SML;
            } else if (isSpace(current)) {
                push_clear_ifn_empty(node);
                continue;//don't push spaces
            } else if (isStringSymbol(current)) {//starting of string
                push_clear_ifn_empty(node);//push anything before the string
                node.token = Node::T_STR;
                bool wasBackSlash = false;
                for (++it; it < end; ++it) {
                    char c = *it;
                    if(c == current && !wasBackSlash){//end of the string
                        break;
                    }
                    if(isBackSlash(c)){
                        wasBackSlash = !wasBackSlash;
                    }else{
                        wasBackSlash = false;
                    }
                    node.str += c;//push each decoded character of the string
                }
                if(it >= end){
                    throw std::runtime_error("ending string not found");
                }
                //string doesn't contain beginning and ending '"'
                push_clear(node);
                continue;//don't push the end
            } else if (isNumber(current)) {
                if (node.token == Node::T_ID) {
                    //nothing to do because ID can hold numbers
                } else if (node.token != Node::T_NUM) {//push what ever were before this
                    push_clear_ifn_empty(node);
                    node.token = Node::T_NUM;
                }
            } else if (isLetter(current)) {
                if (node.token != Node::T_ID) {//push clear what ever were before this
                    push_clear_ifn_empty(node);
                    node.token = Node::T_ID;
                }
            } else if (isOperator(current)) {//operators only have one length characters
                push_clear_ifn_empty(node);
                node.token = Node::T_OP;
            } else {
                throw std::runtime_error(std::string("unexcepted '") + current + "'");
            }
            node.str += current;//push current character
        }
        push_clear_ifn_empty(node);//push the last lexed element if it exists
    }

    Lexer::Lexer(Lexer &&lexer)  noexcept : holder(std::move(lexer.holder)) {
        //empty
    }

    std::string Lexer::toString() const {//for debugging
        std::string result = "[\n";

        for (const auto &node: this->holder) {
            result += node.toString();
            result += ",\n";
        }
        if (this->holder.empty()) {
            result.back() = ']';
        } else {
            result.replace(result.end() - 2, result.end(), "\n]");
        }
        return result;
    }

    void Lexer::push_clear(Node &node) {
        switch(node.token){
            case Node::T_ID:{//checks and pushes keywords with setting sToken
                auto result = findKeyword(node.str);//result = end-index of keyword
                if(result){
                    node.token = Node::T_KW;//token to keyword
                    node.sToken = *(KW_ST_END-result);//setting sToken to the keyword sToken by result
                }
                break;
            }
            case Node::T_SML:{//checks and pushes symbols with setting sToken
                auto result = findSymbol(node.str);
                if(result){
                    node.sToken = *(SML_ST_END-result);
                }
                break;
            }
            case Node::T_STR://nothing to do
            case Node::T_NUM:
            case Node::T_OP:
            case Node::T_KW:
                break;
            case Node::T_NONE:
            default:
                throw std::runtime_error("unknown token in Lexer::push_clear");
        }
        std::swap(holder.emplace_back(),node);//to make node clear and push in the smae time
    }

    void Lexer::push_clear_ifn_empty(Node &node) {
        if (!node.isClear())//if node is not empty
            push_clear(node);
    }

    bool Lexer::isNewLine(char c) {
        return c == '\n';
    }

    bool Lexer::isSpace(char c) {
        switch (c) {
            case ' ':
            //case '\n'://it is used in isNewLine and isSymbol
            case '\t':
            case '\v':
            case '\f':
            case '\r':
                return true;
            default:
                return false;
        }
    }

    bool Lexer::isStringSymbol(char c) {
        return c == '\"' || c == '\'';
    }

    bool Lexer::isNumber(char c) {
        return c >= '0' && c <= '9';
    }

    bool Lexer::isLetter(char c) {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_' || c == '$';
    }

    bool Lexer::isOperator(char c) {
        switch (c) {
            case '(':
            case ')':
            case '=':
            case '+':
            case '-':
            case ',':
            case '.':
            case '*':
                return true;
            default:
                return false;
        }
    }

    bool Lexer::isSymbol(char c) {
        switch (c) {
            case '\n':
            case ';':
            case ':':
            case '{':
            case '}':
                return true;
            default:
                return false;
        }
    }

    bool Lexer::isBackSlash(char c) {
        return c == '\\';
    }

    bool Lexer::isSlash(char c) {
        return c == '/';
    }

    long Lexer::findKeyword(const std::string &str) {
        return KW_STR_END - std::find(KW_STR_BEGIN,KW_STR_END,str);
    }

    long Lexer::findSymbol(const std::string &str) {
        return SML_STR_END - std::find(SML_STR_BEGIN,SML_STR_END,str);
    }
}//namespace felan
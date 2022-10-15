//
// Created by a404m on 8/24/22.
//

#ifndef FELAN_NODE_H
#define FELAN_NODE_H

#include <vector>
#include <stdexcept>
#include <string>
#include <memory>

namespace felan {
    class Node  {
    public:
        friend bool operator<(const Node &node1,const Node &node2); //used for std::map
        //general token which indicates what general kind each
        //operator has
        enum Token {
            T_NONE,//for error indicating and marking empty
            T_STR,//string
            T_NUM,//number
            T_ID,//identifier
            T_OP,//operator
            T_SML,//symbol
            T_KW,//keywords
        };
        //special token which indicates how to parse a node
        enum SpecialToken {
            ST_NONE,//for no parsing
            ST_EOL,//end of file
            ST_LEFT,//needs left side like a++
            ST_RIGHT,//needs right side like ++a
            ST_BOTH,//needs both sides like a+b
            ST_BETWEEN,//needs what ever is between like (a)
            ST_FUN_CALL,//fun call needs name and a like of arguments
            ST_CLASS,//class parsing
            ST_FUN,//fun parsing
            ST_VAR,//variable parsing
            ST_FROM,//from {packages} import {* | ids}
        };

        std::string str;//string of the operator
        Token token;//general token
        SpecialToken sToken;//special token
        std::vector<Node> operands;//operands

        explicit Node(std::string _str = {}, Token _token = T_NONE, SpecialToken _sToken = ST_NONE, decltype(operands) _operands = {});
        Node(const Node &node) = default;//copy constructor
        Node(Node &&node) noexcept;//move constructor

        Node &operator=(const Node &node) = default;//copy assign
        Node &operator=(Node &&node) noexcept = default;//move assign

        std::string toString() const;//toString (for testing)

        inline bool isClear() const {
            return token == T_NONE;//just check this to make it fast
        }

        void clear(Token newToken = T_NONE);//you can set and clear if you change newToken

        inline bool generalEquals(const Node &node) const{
            return this->token == node.token && this->str == node.str;//first compare token to make it faster
        }
        inline bool equals(const Node &node) const{//no need to compare operands
            return  this->token == node.token &&
                    this->sToken == node.sToken &&
                    this->str == node.str;
        }

    private:
        inline static std::string tokenToString(Token _token);//for testing

    public:
        //list of all language nodes
        static const Node SML_NEWLINE;// \n eof
        static const Node SML_SEMICOLON;// ; eof
        static const Node SML_COLON;// : for fun and var and class and stuff

        static const Node BL_CURLY_BRACKET_OPEN;// { for code blocks
        static const Node BL_CURLY_BRACKET_CLOSE;// }
        static const Node BL_PARENTHESES_OPEN;// ( for fun calls and operator ordering and stuff
        static const Node BL_PARENTHESES_CLOSE;// )

        static const Node OP_DOT;// for member accessing of var or class or package
        static const Node OP_FUN_CALLER;// calling a fun by ()
        static const Node OP_POSITIVE;// +a
        static const Node OP_NEGATIVE;// -a
        static const Node OP_PLUS;// a+b
        static const Node OP_MINUS;// a-b
        static const Node OP_ASSIGN;// a=b
        static const Node OP_COMMA;// for functions and arrays

        static const Node KW_PACKAGE;// package {father...}.{packname}
        static const Node KW_IMPORT;// used for `from`
        static const Node KW_FROM;// from ... import ...
        static const Node KW_CLASS;// class
        static const Node KW_FUN;// fun
        static const Node KW_VAR;// var (changeable)
        static const Node KW_VAL;// val (unchangeable)
        static const Node KW_RETURN;// return with or without value
        static const Node KW_PUBLIC;// attributes
        static const Node KW_PRIVATE;
        static const Node KW_PROTECT;
        static const Node KW_OPEN;
        static const Node KW_CLOSE;
        static const Node KW_NATIVE;
        static const Node KW_BUILTIN;
        static const Node KW_OVERRIDE;
        static const Node KW_OPERATOR;
        static const Node KW_STATIC;

        static const Node NONE;// used for empty but parsed blocks
    };
}//namespace felan

#endif //FELAN_NODE_H

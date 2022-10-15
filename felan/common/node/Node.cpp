//
// Created by a404m on 8/24/22.
//

#include "Node.h"

namespace felan {
    const Node Node::SML_NEWLINE{"\n",T_SML,ST_EOL};
    const Node Node::SML_SEMICOLON{";",T_SML,ST_EOL};
    const Node Node::SML_COLON{":",T_SML,ST_NONE};

    const Node Node::BL_CURLY_BRACKET_OPEN{"{",T_SML,ST_BETWEEN};
    const Node Node::BL_CURLY_BRACKET_CLOSE{"}",T_SML,ST_BETWEEN};
    const Node Node::BL_PARENTHESES_OPEN{"(",T_OP,ST_BETWEEN};
    const Node Node::BL_PARENTHESES_CLOSE{")",T_OP,ST_BETWEEN};

    const Node Node::OP_DOT{".",T_OP,ST_BOTH};
    const Node Node::OP_FUN_CALLER{"(",T_OP,ST_FUN_CALL};
    const Node Node::OP_POSITIVE{"+",T_OP,ST_RIGHT};
    const Node Node::OP_NEGATIVE{"-",T_OP,ST_RIGHT};
    const Node Node::OP_PLUS{"+",T_OP,ST_BOTH};
    const Node Node::OP_MINUS{"-",T_OP,ST_BOTH};
    const Node Node::OP_ASSIGN{"=",T_OP,ST_BOTH};
    const Node Node::OP_COMMA{",",T_OP,ST_BOTH};

    const Node Node::KW_PACKAGE{"package",T_KW,ST_RIGHT};
    const Node Node::KW_IMPORT{"import",T_KW,ST_NONE};
    const Node Node::KW_FROM{"from",T_KW,ST_FROM};
    const Node Node::KW_CLASS{"class",T_KW,ST_CLASS};
    const Node Node::KW_FUN{"fun",T_KW,ST_FUN};
    const Node Node::KW_VAR{"var",T_KW,ST_VAR};
    const Node Node::KW_VAL{"val",T_KW,ST_VAR};
    const Node Node::KW_RETURN{"return",T_KW,ST_RIGHT};
    const Node Node::KW_PUBLIC{"public",T_KW,ST_NONE};
    const Node Node::KW_PRIVATE{"private",T_KW,ST_NONE};
    const Node Node::KW_PROTECT{"protect",T_KW,ST_NONE};
    const Node Node::KW_OPEN{"open",T_KW,ST_NONE};
    const Node Node::KW_CLOSE{"close",T_KW,ST_NONE};
    const Node Node::KW_NATIVE{"native",T_KW,ST_NONE};
    const Node Node::KW_BUILTIN{"builtin",T_KW,ST_NONE};
    const Node Node::KW_OVERRIDE{"override",T_KW,ST_NONE};
    const Node Node::KW_OPERATOR{"operator",T_KW,ST_NONE};
    const Node Node::KW_STATIC{"static",T_KW,ST_NONE};

    const Node Node::NONE{"",T_NONE,ST_NONE};

    Node::Node(std::string _str, Token _token, SpecialToken _sToken, decltype(operands) _operands) :
        str(std::move(_str)), token(_token), sToken(_sToken), operands(std::move(_operands)) {
        //empty
    }

    Node::Node(Node &&node) noexcept: str(std::move(node.str)), token(node.token), sToken(node.sToken), operands(std::move(node.operands)) {
        //empty
    }

    std::string Node::toString() const {//for debugging
        return "{\"" + str + "\"," + tokenToString(token) + "}";
    }

    void Node::clear(Token newToken) {
        str.clear();
        token = newToken;
    }

    std::string Node::tokenToString(Node::Token _token) {//for debugging
        switch (_token) {
            case T_NONE:
                return "T_NONE";
            case T_STR:
                return "T_STR";
            case T_NUM:
                return "T_NUM";
            case T_ID:
                return "T_ID";
            case T_OP:
                return "T_OP";
            case T_SML:
                return "T_SML";
            case T_KW:
                return "T_KW";
            default:
                throw std::runtime_error("unknown token " + std::to_string((int) _token));
        }
    }

    //friends
    bool operator<(const Node &node1,const Node &node2){//used for std::map
        auto s1 = node1.str+(char)node1.token+(char)node1.sToken;
        auto s2 = node2.str+(char)node2.token+(char)node2.sToken;
        return s1<s2;
        /*return (node1.token < node2.token) ||
        (node1.token == node2.token && node1.sToken < node2.sToken) ||
        (node1.token == node2.token && node1.sToken == node2.sToken && node1.str < node1.str)*/;
    }
}//namespace felan


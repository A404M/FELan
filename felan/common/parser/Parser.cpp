//
// Created by a404m on 9/3/22.
//

#include "Parser.h"
#include <algorithm>
#include <utility>

namespace felan {
    const std::vector<std::pair<std::vector<Node>,bool>> Parser::PARSE_ORDER = {
            {{Node::BL_PARENTHESES_OPEN,Node::BL_CURLY_BRACKET_OPEN},false},
            {{Node::SML_NEWLINE,Node::SML_SEMICOLON},true},
            {{Node::OP_DOT},false},
            {{Node::KW_PACKAGE,Node::KW_FROM},true},
            {{Node::OP_FUN_CALLER},false},
            {{Node::OP_POSITIVE,Node::OP_NEGATIVE},false},
            {{Node::OP_PLUS,Node::OP_MINUS},true},
            {{Node::KW_VAR,Node::KW_VAL},true},
            {{Node::OP_ASSIGN},false},
            {{Node::KW_RETURN},false},
            //{{Node::OP_COMMA},true},
            {{Node::KW_CLASS,Node::KW_FUN},true},
    };

    Parser::Parser(Lexer lexer) : holder() {
        auto begin = lexer.holder.begin(),end = lexer.holder.end();
        parse(lexer.holder,begin,end);
    }

    void Parser::parse(Lexer::Holder &lHolder, Lexer::Holder::iterator &begin, Lexer::Holder::iterator &end) {
        parseWithoutPush(lHolder,begin,end);
        if(end-begin > 1){
            throw std::runtime_error("no way to parse in Parser::parse");
        }else if(end-begin == 1){
            this->holder.emplace_back(std::move(*begin));
        }
    }

    void Parser::parseWithoutPush(Lexer::Holder &lHolder, Lexer::Holder::iterator &begin, Lexer::Holder::iterator &end){
        while(begin < end){
            auto it = findNextInOrder(begin,end);
            if(it == end){
                break;
            }
            parseByST(lHolder,begin,end,it);
        }
    }

    Lexer::Holder::iterator Parser::findNextInOrder(Lexer::Holder::iterator begin, Lexer::Holder::iterator end) {
        for(auto it = begin;it < end;++it){
            if(!isOperand(*it))
                setST(begin,end,it);
        }
        for(const auto &order : PARSE_ORDER){
            if(order.second){
                for(auto it = begin; it < end; ++it){
                    if(!isOperand(*it)) {
                        if (std::find_if(order.first.begin(), order.first.end(), [it](const Node &node) -> bool {
                            return it->equals(node);
                        }) != order.first.cend()) {
                            return it;
                        }
                    }
                }
            }else{
                for(auto it = end - 1; it >= begin; --it){
                    if(!isOperand(*it)) {
                        if (std::find_if(order.first.begin(), order.first.end(), [it](const Node &node) -> bool {
                            return it->equals(node);
                        }) != order.first.cend()) {
                            return it;
                        }
                    }
                }
            }
        }
        return end;
    }

    void Parser::parseByST(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end, Lexer::Holder::iterator it) {
        switch(it->sToken){
            case Node::ST_EOL:
                nodeEOLParser(lHolder,begin,end,it);
                break;
            case Node::ST_LEFT:
                nodeLeftParser(lHolder,it);
                --end;
                break;
            case Node::ST_RIGHT:
                nodeRightParser(lHolder,it);
                --end;
                break;
            case Node::ST_BOTH:
                nodeBothParser(lHolder,it);
                end -= 2;
                break;
            case Node::ST_BETWEEN:
                nodeBetweenParser(lHolder,begin,end,it);
                break;
            case Node::ST_CLASS:
                nodeClassParser(lHolder,begin,end,it);
                break;
            case Node::ST_FUN:
                nodeFunParser(lHolder,begin,end,it);
                break;
            case Node::ST_VAR:
                nodeVarParser(lHolder,begin,end,it);
                break;
            case Node::ST_FUN_CALL:
                nodeFunCallParser(lHolder,begin,end,it);
                break;
            case Node::ST_FROM:
                nodeFromParser(lHolder,begin,end,it);
            case Node::ST_NONE:
                break;
            default:
                throw std::runtime_error("unknown sToken in Parser::parseByST");
        }
    }

    void Parser::setST(Lexer::Holder::iterator begin, Lexer::Holder::iterator end, Lexer::Holder::iterator it) {
        if(it->token == Node::T_KW || it->token == Node::T_SML){
            return;
        }if(it->generalEquals(Node::BL_PARENTHESES_OPEN)){
            if(isOperand(*(it-1))){
                if((it-2)->generalEquals(Node::KW_FUN)) {
                    it->sToken = Node::ST_NONE;
                }else{
                    it->sToken = Node::ST_FUN_CALL;
                }
            }else{
                it->sToken = Node::ST_BETWEEN;
            }
        }else if(it != begin && isOperand(*(it-1))){
            if(it+1 < end && isOperand(*(it+1))){
                it->sToken = Node::ST_BOTH;
            }else{
                it->sToken = Node::ST_LEFT;
            }
        }else{
            if(it+1 < end && isOperand(*(it+1))){
                it->sToken = Node::ST_RIGHT;
            }else{
                it->sToken = Node::ST_NONE;
            }
        }
    }

    bool Parser::isOperand(const Node &node) {
        if(!node.operands.empty())
            return true;
        switch (node.token) {
            case Node::T_STR:
            case Node::T_NUM:
            case Node::T_ID:
                return true;
            case Node::T_OP:
                return !node.operands.empty();
            case Node::T_SML:
            case Node::T_KW:
            case Node::T_NONE:
                return false;
            default:
                throw std::runtime_error("unknown token in Parser::isOperand");
        }
    }

    void Parser::nodeEOLParser(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end, Lexer::Holder::iterator it) {
        it = lHolder.erase(it);
        --end;
        auto diff = it-begin;
        parseWithoutPush(lHolder,begin,it);
        end-=diff-(it-begin);
        if(it-begin == 1){
            this->holder.push_back(std::move(*begin));
            begin = lHolder.erase(begin);
            --end;
        }else if(it-begin > 1){
            throw std::runtime_error("no way to parse this expression");
        }
    }

    void Parser::nodeLeftParser(Lexer::Holder &lHolder, Lexer::Holder::iterator it) {
        it->operands.emplace_back(std::move(*(it-1)));
        lHolder.erase(it-1);
    }

    void Parser::nodeRightParser(Lexer::Holder &lHolder, Lexer::Holder::iterator it) {
        it->operands.emplace_back(std::move(*(it+1)));
        lHolder.erase(it+1);
    }

    void Parser::nodeBothParser(Lexer::Holder &lHolder, Lexer::Holder::iterator it) {
        std::swap(*it,*(it-1));
        --it;
        it->operands = {std::move(*(it+1)),std::move(*(it+2))};
        lHolder.erase(it+1,it+3);
    }

    void Parser::nodeBetweenParser(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end, Lexer::Holder::iterator it) {
        const Node &close = (it->equals(Node::BL_PARENTHESES_OPEN))?
                            Node::BL_PARENTHESES_CLOSE:
                            //(is->equals(Node::BL_CURLY_BRACKET_OPEN))?
                            Node::BL_CURLY_BRACKET_CLOSE
                            //: nullptr
                            ;
        auto closeEnd = std::find_if(it,lHolder.end(),[close](const Node &n)->bool{
            return close.generalEquals(n);
        });
        if(closeEnd == end){
            throw std::runtime_error("no closing " + close.str);
        }
        auto b = it+1;
        if(closeEnd-b) {
            auto diff = closeEnd-b;
            auto bSize = this->holder.size();
            parseWithoutPush(lHolder, b, closeEnd);
            move(this->holder,it->operands,this->holder.begin()+bSize,this->holder.end());
            if(closeEnd-b)
                it->operands.emplace_back(std::move(*b));
            end -= diff-(closeEnd-b);
        }
        lHolder.erase(b,closeEnd+1);
        end -= (closeEnd+1)-b;
        if(it->operands.empty()){
            it->operands.emplace_back(Node::NONE);
        }
    }

    void Parser::nodeClassParser(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end, Lexer::Holder::iterator it){
        auto b = parseAttribute(*it,begin,end,it-1);

        if(it+2 >= end){
            throw std::runtime_error("bad syntax for class");
        }

        if((it+1)->token == Node::T_ID){//class name
            it->operands.emplace_back(std::move(*(it+1)));
        }else{
            throw std::runtime_error("class needs a name");
        }

        if((it+2)->generalEquals(Node::BL_CURLY_BRACKET_OPEN)){
            it->operands.emplace_back(std::move(*(it+2)));
        }else{
            throw std::runtime_error("class needs body");
        }

        if(it != b)
            std::swap(*b,*it);
        lHolder.erase(b+1,it+3);
        end -= (it+3)-(b+1);
    }

    void Parser::nodeFunParser(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end, Lexer::Holder::iterator it){
        auto b = parseAttribute(*it,begin,end,it-1);

        if(it+2 >= end){
            E_WRONG_STX:
            throw std::runtime_error("wrong syntax for fun");
        }

        //todo change this to be able to use lambdas
        if(isOperand(*(it+1))){//fun name
            it->operands.emplace_back(std::move(*(it+1)));
        }else{
            throw std::runtime_error("fun needs a name");
        }

        if((it+2)->generalEquals(Node::BL_PARENTHESES_OPEN)){
            auto tb = it+3;
            auto temp = it+2;
            parseParams(lHolder,tb,end,*temp);
            it->operands.emplace_back(std::move(*temp));
        }else{
            throw std::runtime_error("fun needs parameter list");
        }

        //is+3 = ) but it is deleted

        int eDiff = 4;

        if(it+3 >= end){
            eDiff = 3;
        }else if((it+3)->generalEquals(Node::BL_CURLY_BRACKET_OPEN)){
            it->operands.emplace_back(std::move(*(it + 3)));
        }else if((it+3)->generalEquals(Node::SML_COLON)){
            if(it+4 >= end){
                goto E_WRONG_STX;
            }
            if((it+4)->token == Node::T_ID){
                it->operands.emplace_back(std::move(*(it+4)));
            }else{
                throw std::runtime_error("no return type after : for fun");
            }
            if(it+5 < end && (it+5)->generalEquals(Node::BL_CURLY_BRACKET_OPEN)){
                it->operands.emplace_back(std::move(*(it+5)));
                eDiff = 6;
            }else{
                eDiff = 5;
            }
        }else{
            throw std::runtime_error("fun needs body");
        }

        if(b != it)
            std::swap(*b,*it);
        lHolder.erase(b+1,it+eDiff);
        end -= (it+eDiff)-(b+1);
    }

    void Parser::nodeVarParser(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end, Lexer::Holder::iterator it) {
        auto b = parseAttribute(*it,begin,end,it-1);

        if(it+3 >= end){
            throw std::runtime_error("var syntax is wrong");
        }
        if((it+1)->token != Node::T_ID){
            throw std::runtime_error("var needs an ID");
        }
        if(!(it+2)->generalEquals(Node::SML_COLON)){
            throw std::runtime_error("var needs a :");
        }
        if((it+3)->token != Node::T_ID){
            throw std::runtime_error("var needs a type");
        }
        it->operands = {std::move(*(it+1)),std::move(*(it+3))};
        /*if(it+4 < end && (it+4)->generalEquals(Node::OP_ASSIGN)){//=
            if(it+5 < end) {
                auto tBegin = it+5;
                parseWithoutPush(lHolder,tBegin,end);
                it->operands.emplace_back(std::move(*tBegin));
            }else{
                throw std::runtime_error("= needs a value in var");
            }
            if(it != b)
                std::swap(*b,*it);
            lHolder.erase(b+1,it+6);
            end-=(it+6)-(b+1);
        }else{
            if(it != b)
                std::swap(*b,*it);
            lHolder.erase(b+1,it+4);
            end-=(it+4)-(b+1);
        }
        */
        if(it != b)
            std::swap(*b,*it);
        lHolder.erase(b+1,it+4);
        end-=(it+4)-(b+1);
    }

    void Parser::nodeFunCallParser(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end, Lexer::Holder::iterator it){
        it->operands.emplace_back(std::move(*(it-1)));
        *(it-1) = std::move(*it);
        --it;
        auto b = it+2;
        parseParams(lHolder,b,end,*it);

        lHolder.erase(it+1);
        --end;
    }

    void Parser::nodeFromParser(Lexer::Holder &lHolder, std::vector<Node>::iterator &begin,
                                std::vector<Node>::iterator &end,Lexer::Holder::iterator it) {
        auto t = it+1;
        if(t->token == Node::T_ID || t->equals(Node::OP_DOT)){
            it->operands.emplace_back(std::move(*t));
        }else{
            throw std::runtime_error("no path for from");
        }
        ++t;
        if(t->generalEquals(Node::KW_IMPORT)){
            ++t;
            if(t == end){
                throw std::runtime_error("import needs param");
            }
            if(t+1 == end && it->str == "*"){
                ++t;
                it->operands.emplace_back(std::move(*t));
                ++t;
            }else {
                auto b = t;
                for (; t < end; ++t) {
                    if (t->generalEquals(Node::OP_COMMA)) {
                        if (b + 1 != t || b->token != Node::T_ID) {
                            throw std::runtime_error("bad syntax in import param");
                        }
                        it->operands.emplace_back(std::move(*b));
                        b = t + 1;
                    }
                }
                it->operands.emplace_back(std::move(*b));
            }
        }else{
            throw std::runtime_error("from needs import");
        }
        lHolder.erase(it+1,t);
        end -= t-(it+1);
    }

    void Parser::parseParams(Lexer::Holder &lHolder, std::vector<Node>::iterator &begin, std::vector<Node>::iterator &end,Node &node) {
        if(begin->generalEquals(Node::BL_PARENTHESES_CLOSE)) {
            lHolder.erase(begin);
            --end;
            return;
        }
        auto b = begin;
        for(auto e = b;;++e){
            if(e->generalEquals(Node::OP_COMMA)){
                parseWithoutPush(lHolder,b,e);
                node.operands.emplace_back(std::move(*b));
                b = e+1;
            }else if(e->generalEquals(Node::BL_PARENTHESES_CLOSE)){
                parseWithoutPush(lHolder,b,e);
                node.operands.emplace_back(std::move(*b));
                b = e+1;
                break;
            }
            if(e >= end){
                throw std::runtime_error("no closing () for fun");
            }
        }
        auto diff = b-begin;
        lHolder.erase(begin,b);
        end -= diff;
    }

    Lexer::Holder::iterator Parser::parseAttribute(Node &node,Lexer::Holder::iterator begin,Lexer::Holder::iterator end,Lexer::Holder::iterator it) {
        static const std::string ATTRIBUTE_STR[] = {
                "public","private","protect","open","close",
                "native","builtin","override","operator","static"
        };
        static const auto ATTRIBUTE_BEGIN = std::begin(ATTRIBUTE_STR);
        static const auto ATTRIBUTE_END = std::end(ATTRIBUTE_STR);

        for(; it >= begin; --it){
            if(it->token == Node::T_STR){
                if(it - 1 >= begin && (it - 1)->token == Node::T_KW && (it - 1)->str == "native"){
                    --it;
                    it->operands.emplace_back(std::move(*(it+1)));
                    goto E_BACK;
                }else{
                    break;
                }
            }
            if(it->token != Node::T_KW ||
            std::find(ATTRIBUTE_BEGIN, ATTRIBUTE_END, it->str) == ATTRIBUTE_END){
                break;
            }
            E_BACK:
            node.operands.emplace_back(std::move(*it));
        }
        std::reverse(node.operands.begin(), node.operands.end());
        ++it;

        return it;
    }

    template<class H, class Iter>
    void Parser::move(H &h1, H &h2, Iter begin, Iter end) {
        h2.insert(h2.end(), std::make_move_iterator(begin), std::make_move_iterator(end));
        h1.erase(begin,end);
    }

} // felan
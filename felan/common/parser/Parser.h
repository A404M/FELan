//
// Created by a404m on 9/3/22.
//

#ifndef FELAN_PARSER_H
#define FELAN_PARSER_H

#include "felan/common/lexer/Lexer.h"

namespace felan {

    class Parser {
        friend class MakePackage;
        friend class CompilerFrontend;
        friend class Runner;
    private:
        typedef std::vector<std::pair<std::vector<Node>,bool>> ParseOrder;
        typedef std::vector<Node> Holder;
        Holder holder;

        static const ParseOrder PARSE_ORDER;
    public:
        explicit Parser(Lexer lexer);

        Parser(const Parser &parser) = default;
        Parser(Parser &&parser) = default;

        Parser &operator=(const Parser &parser) = default;
        Parser &operator=(Parser &&parser) = default;
    private:
        inline void parse(Lexer::Holder &lHolder, Lexer::Holder::iterator &begin, Lexer::Holder::iterator &end);
        inline void parseWithoutPush(Lexer::Holder &lHolder, Lexer::Holder::iterator &begin, Lexer::Holder::iterator &end);

        inline static Lexer::Holder::iterator findNextInOrder(Lexer::Holder::iterator begin,Lexer::Holder::iterator end);

        inline void parseByST(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end,Lexer::Holder::iterator it);

        inline static void setST(Lexer::Holder::iterator begin,Lexer::Holder::iterator end,Lexer::Holder::iterator it);
        static bool isOperand(const Node &node);

        void nodeEOLParser(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end,Lexer::Holder::iterator it);
        inline static void nodeLeftParser(Lexer::Holder &lHolder, Lexer::Holder::iterator it);
        inline static void nodeRightParser(Lexer::Holder &lHolder, Lexer::Holder::iterator it);
        inline static void nodeBothParser(Lexer::Holder &lHolder, Lexer::Holder::iterator it);
        void nodeBetweenParser(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end, Lexer::Holder::iterator it);
        void nodeClassParser(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end, Lexer::Holder::iterator it);
        void nodeFunParser(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end, Lexer::Holder::iterator it);
        void nodeVarParser(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end, Lexer::Holder::iterator it);
        void nodeFunCallParser(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end, Lexer::Holder::iterator it);
        void nodeFromParser(Lexer::Holder &lHolder,Lexer::Holder::iterator &begin,Lexer::Holder::iterator &end, Lexer::Holder::iterator it);

        inline void parseParams(Lexer::Holder &lHolder, Lexer::Holder::iterator &begin, Lexer::Holder::iterator &end, Node &node);
        static inline Lexer::Holder::iterator parseAttribute(Node &node,Lexer::Holder::iterator begin,Lexer::Holder::iterator end,Lexer::Holder::iterator it);

        template<class H,class Iter>
        inline static void move(H &h1,H &h2,Iter begin,Iter end);
    };

} // felan

#endif //FELAN_PARSER_H

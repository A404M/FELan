//
// Created by a404m on 9/19/22.
//

#ifndef FELAN_EXPRESSION_H
#define FELAN_EXPRESSION_H

#include <map>
#include <felan/common/package/Package.h>

namespace felan {
    class Variable;
    class Fun;
    class MakePackage;
    class Expression {
    public:
        typedef std::map<Node,std::string>  OpToStr;
    private:
        static const OpToStr opToStr;
    public:
        struct Operand{
            enum Kind {
                NONE = 0,
                INT,
                STRING,
                EXPRESSION,
                VARIABLE,
            };
            bool owns;
            Kind kind;
            void *pointer;

            Operand(void *_pointer,Kind _kind,bool _owns);
            Operand(Operand &&op) noexcept ;

            Operand &operator=(Operand &&op) noexcept ;

            ~Operand();

            Class *getType(MakePackage *mp) const;
        };

        Fun *fun;
        std::vector<Operand> operands;

        Expression(Node &node,MakePackage *mp,Fun *parentFun);

    private:
        void doVarOperand(std::string_view varName,MakePackage *mp,Fun *parentFun);
        static Package::Element *doDot(Node &n, MakePackage *mp, Fun *parentFun);
        static std::pair<Package::Element *,std::string> doBeforeDot(Node &n, MakePackage *mp, Fun *parentFun);
        static std::pair<std::string,std::string> dotToString(Node &n);
    };

} // felan

#endif //FELAN_EXPRESSION_H

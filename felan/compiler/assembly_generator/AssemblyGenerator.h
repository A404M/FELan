//
// Created by a404m on 10/14/22.
//

#ifndef FELAN_ASSEMBLYGENERATOR_H
#define FELAN_ASSEMBLYGENERATOR_H

#include <felan/compiler/make_package/MakePackage.h>
#include <felan/common/parent/Parent.h>
#include <felan/common/expression/Expression.h>
#include <map>

namespace felan {

    class AssemblyGenerator {
    private:
        Fun *mainFun;
        std::map<void *,std::string> names;
        std::map<std::string,std::string> constValues;
        std::vector<std::string> subRoutines;

        std::vector<std::string> functions;
    public:
        explicit AssemblyGenerator(Fun *_mainFun);

        std::string compile();
    private:
        std::string callFun(Fun *fun);
        void compileFun(Fun *fun);

        std::string compileExpression(Expression *expr);

        static std::string pushVar(Variable *var);
        std::string pushOperand(Expression::Operand &operand);
        static std::string popVar(Variable *var);

        static std::string getName(Parent parent);
        static std::string getName(Class *clas);
        static std::string getName(Fun *fun);
        static std::string getName(Variable *var);

        void addConst(std::string key,std::string value);

        std::string getSign(Fun *fun);
    };

} // felan

#endif //FELAN_ASSEMBLYGENERATOR_H

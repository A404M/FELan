//
// Created by a404m on 9/12/22.
//

#ifndef FELAN_FUN_H
#define FELAN_FUN_H

#include <felan/common/node/Node.h>
#include <felan/common/variable/Variable.h>

namespace felan {
    class Expression;
    class Fun {
        friend class Parent;
        friend class Package;
    public:
        Attribute attribute;
        std::string name;
        std::vector<Class *> arguments;
        std::vector<Variable*> vars;
        Class *retType = nullptr;
        std::vector<Expression> body;
        Parent parent = Parent();
        int64_t paramSize;
        Node bodyNode;


        Fun(Node &node,MakePackage *mp,Parent _parent);
        explicit Fun(std::string _name);
        Fun(const Fun &fun) = delete;
        Fun(Fun &&fun) noexcept = delete;

        ~Fun();

        Fun &operator=(const Fun &fun) = delete;
        Fun &operator=(Fun &&fun) noexcept = delete;

        [[nodiscard]] bool isIncomplete() const;

        void completeBody(MakePackage *mp);

        Variable *findVar(std::string_view varName);
        Variable *addVar(Node &node, MakePackage *mp);

        int64_t getVarID(Variable *var);

        friend bool operator==(const Fun &f1,const Fun &f2);

    public:
        static Fun assign;
    };

} // felan

#endif //FELAN_FUN_H

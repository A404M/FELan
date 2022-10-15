//
// Created by a404m on 9/11/22.
//

#ifndef FELAN_VARIABLE_H
#define FELAN_VARIABLE_H

#include <felan/common/node/Node.h>
#include <felan/common/parent/Parent.h>
#include <map>
#include <felan/common/error/syntax_error/SyntaxError.h>
#include <felan/common/error/not_found_error/NotFoundError.h>
#include <felan/common/attribute/Attribute.h>

namespace felan {
    class MakePackage;
    class Variable {
        friend class Package;
        friend class Parent;
        friend class AssemblyGenerator;
    public:
        Attribute attribute;
        std::string name;
        Class *type = nullptr;
        Parent parent = Parent();

        Variable(Node &node,MakePackage *mp,Parent _parent);
        explicit Variable(std::string _name);
        Variable(const Variable &) = delete;
        Variable(Variable &&) = delete;

        ~Variable();

        Variable &operator=(const Variable&) = delete;
        Variable &operator=(Variable&&) = delete;

        [[nodiscard]] bool isIncomplete() const;

        friend bool operator==(const Variable &var1,const Variable &var2);
    };

} // felan

#endif //FELAN_VARIABLE_H

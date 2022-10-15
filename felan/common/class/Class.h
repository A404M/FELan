//
// Created by a404m on 9/12/22.
//

#ifndef FELAN_CLASS_H
#define FELAN_CLASS_H

#include <felan/common/variable/Variable.h>
#include <felan/common/fun/Fun.h>
#include <felan/common/package/Package.h>
#include <felan/common/error/syntax_error/SyntaxError.h>
#include <felan/common/attribute/Attribute.h>

namespace felan {

    class Class {
        friend class Parent;
        friend class AssemblyGenerator;
    public:
        Attribute attribute;
        std::string name;
        Parent father;
        std::vector<Package::Element> members;
        std::vector<Node> bodyNodes;

        Class(Node &node, MakePackage *mp, Package *_father);
        explicit Class(std::string _name);

        void makeBody(MakePackage *mp);
        void completeBody(MakePackage *mp);

        bool isIncomplete() const;

        Fun *getMethod(std::string_view str, std::vector<Class *> arguments);

        Package::Element *findAny(std::string_view elName);

        friend bool operator==(const Class &clas1,const Class &clas2);
    };

} // felan

#endif //FELAN_CLASS_H

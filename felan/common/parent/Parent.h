//
// Created by a404m on 9/25/22.
//

#ifndef FELAN_PARENT_H
#define FELAN_PARENT_H

#include <string>

namespace felan {
    class Package;
    class Class;
    class Fun;
    class Parent {
    public:
        enum Kind{
            NONE,
            PACKAGE,
            CLASS,
            FUN
        };
        void *pointer = nullptr;
        Kind kind = NONE;

        Parent(void *_pointer,Kind _kind);
        explicit Parent(Package *package);
        explicit Parent(Class *clas);
        explicit Parent(Fun *fun);
        explicit Parent();
        Parent(const Parent &) = default;
        Parent(Parent &&parent) noexcept = default;

        Parent &operator=(const Parent &) = default;
        Parent &operator=(Parent &&parent) noexcept = default;

        Parent getParent() const;
        std::string getName() const;
    };

} // felan

#endif //FELAN_PARENT_H

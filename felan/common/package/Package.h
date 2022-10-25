//
// Created by a404m on 9/12/22.
//

#ifndef FELAN_PACKAGE_H
#define FELAN_PACKAGE_H

#include <felan/common/node/Node.h>
#include <felan/common/parent/Parent.h>

namespace felan {
    class Variable;
    class Fun;
    class Class;
    class Package {
        friend class Parent;
        friend class AssemblyGenerator;
    public:
        std::string name;
        Parent father;

        class Element{
        public:
            enum Kind{
                NONE = 0,
                CLASS,
                FUN,
                VARIABLE,
                PACKAGE
            };
            void *pointer = nullptr;
            Kind kind = NONE;
            bool owns = false;

            Element(void *_pointer,Kind _kind,bool _owns);
            Element(const Element &) = delete;
            Element(Element &&element) noexcept ;

            ~Element();

            void clear();

            template<class T>
            T &getAs(){
                return *((T*)this->pointer);
            }

            template<class T>
            [[nodiscard]] T &getAs() const {
                return *((T*)this->pointer);
            }

            [[nodiscard]] bool isIncomplete() const;

            [[nodiscard]] std::string_view getName() const;

            bool equals(void *ptr, Element::Kind kind) const;
            friend bool operator==(const Element &el1,const Element &el2){
                return el1.pointer == el2.pointer && el1.kind == el2.kind;
            }
        };
        typedef std::vector<Element> ElementHolder;

        ElementHolder elements;

        Package(std::string _name,Package *_father,std::vector<Element> _elements = {});
        Package(const Package &) = delete;
        Package(Package &&) = delete;

        Package &operator=(const Package &) = delete;
        Package &operator=(Package &&) = delete;

        void clear(){
            elements.clear();
        }

        Package *pushPackage(std::string packageName);
        template<typename Func>
        Class *pushClass(Node &node,Func stringToClass);
        void *push(void *pointer,Element::Kind kind,bool owns);
        void *pushInComplete(std::string eName,Element::Kind kind);

        Element *find(void *pointer, Element::Kind kind);
        void *find(std::string_view elName, Element::Kind kind);
        Element *findAny(std::string_view elName);
        Class *findInAnyLevel(const std::string &className,Element::Kind kind);
        Element *getClassOrPackageByPath(const std::string &path, Element::Kind kind);

        bool hasPackage(const std::string &packageName);
        bool hasClass(const std::string &className);
        bool hasIncomplete();

        static Package *findIncompletePackage(Package *pack);

        friend bool operator==(const Package &pack1,const Package &pack2);
    };

} // felan

#endif //FELAN_PACKAGE_H

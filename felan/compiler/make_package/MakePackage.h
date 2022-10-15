//
// Created by a404m on 9/14/22.
//

#ifndef FELAN_MAKEPACKAGE_H
#define FELAN_MAKEPACKAGE_H

#include <felan/common/package/Package.h>
#include <felan/common/parser/Parser.h>

namespace felan {

    class CompilerFrontend;
    class MakePackage {
        friend class CompilerFrontend;
        friend class Expression;
        friend class AssemblyGenerator;
    public:
        typedef Package GlobalElements;
    private:
        static Package rootPackage;
        Package *package;
        GlobalElements globalElements;
        CompilerFrontend *cf;
        Parser parser;

    public:
        MakePackage(Parser _parser, const std::string& filePath, CompilerFrontend *_cf);
        MakePackage(const MakePackage &) = delete;
        MakePackage(MakePackage &&) = default;

        MakePackage &operator=(const MakePackage &) = delete;
        MakePackage &operator=(MakePackage &&) = default;

        static void clearRootPackage(){
            rootPackage.clear();
        }

        Class *findClass(std::string_view str);
        Fun *findFun(std::string_view str, std::vector<Class *> arguments);
        Variable *findVar(std::string_view str);
    private:
        void doAll(const std::string& filePath);
        void doAllClasses();
        void doMake();
        void doMakeClassBodies();
        void doComplete();

        Package *doPackage(Node &node,std::string fileName);
        Package *doDotPackage(Node &dot,Package *father);

        void doImports();
        void doImport(Node &node);

        inline void doClass(Node &node,Package &pack);
        inline void doFun(Node &node,Package &pack);
        inline void doVariable(Node &node,Package &pack);

        Package::Element *findGlobal(void *pointer, Package::Element::Kind kind);
        void *findGlobal(std::string_view str, Package::Element::Kind kind);

        Package::Element *findGlobalID(std::string_view str);
        bool isCompleteGlobalIDExist(std::string_view str);
        bool isCompleteGlobalElementExist(void *,Package::Element::Kind kind);
    };

} // felan

#endif //FELAN_MAKEPACKAGE_H

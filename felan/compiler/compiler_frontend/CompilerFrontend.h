//
// Created by a404m on 9/17/22.
//

#ifndef FELAN_COMPILERFRONTEND_H
#define FELAN_COMPILERFRONTEND_H

#include <felan/compiler/make_package/MakePackage.h>
#include <map>

namespace felan {

    class CompilerFrontend {
    public:
        typedef std::map<std::string,Package::Element::Kind> FileElements;
    private:
        std::string projectDir;
        std::map<std::string,std::string> consts;
        std::map<std::string,Parser> fileCache;
        std::map<std::string,MakePackage> makeFiles;

    public:
        explicit CompilerFrontend(std::string_view _projectDir);

        std::string parseStringWithConsts(std::string str);

        std::string compile();
        Parser &getParsed(const std::string &fileName);

        MakePackage &getMakeFile(const std::string &filePath);
        std::string getFilePath(Package const*package);
        static std::string getFileName(std::string_view filePath);
    private:
        static std::string readFile(const std::string &filePath);
        static bool isFileExist(const std::string &filePath);
    };

} // felan

#endif //FELAN_COMPILERFRONTEND_H

//
// Created by a404m on 9/17/22.
//

#include "CompilerFrontend.h"
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <felan/compiler/assembly_generator/AssemblyGenerator.h>
#include <felan/common/fun/Fun.h>
#include <iostream>

namespace felan {
    CompilerFrontend::CompilerFrontend(std::string_view _projectDir) : projectDir(_projectDir){
        std::ifstream buildFile{this->projectDir+"FELanBuild.txt"};
        if(!buildFile){
            throw std::runtime_error("no FELanBuild.txt file found");
        }
        std::string str1,str2;
        while(std::getline(buildFile,str1)){
            std::istringstream is;
            is.str(str1);
            if(!is){
                continue;
            }
            is >> str1 >> str2;
            consts[str1] = parseStringWithConsts(std::move(str2));
            if(!is.eof()){
                throw std::runtime_error("not a pair key");
            }
        }
    }

    CompilerFrontend::~CompilerFrontend() {
        for(auto &pair : makeFiles){
            delete pair.second;
        }
    }

    std::string CompilerFrontend::parseStringWithConsts(std::string str) {
        decltype(str)::iterator it;
        while((it = std::find(str.begin(),str.end(),'$'))!=str.end()){
            auto e = std::find_if(it+1,str.end(),[](char c){
                return c == '/' || c == '.';
            });
            auto key = std::string(it+1,e);
            auto toRep = consts.at(key);
            str.replace(it,e,toRep);
        }
        return str;
    }

    std::string CompilerFrontend::compile() {
        auto mainFile = makeFiles.emplace(
                consts.at("MAIN_FILE"),
                new MakePackage(
                        getParsed(consts.at("MAIN_FILE")),
                        consts.at("MAIN_FILE_NAME"), this)
                );
        for(auto &file : makeFiles){
            file.second->doMakeClassBodies();
        }
        for(auto &file : makeFiles){
            file.second->doComplete();
        }
        Fun mainFunSign{"main"};
        mainFunSign.arguments = {};
        auto mainFunElP = mainFile.first->
                second->globalElements.
                find(&mainFunSign,Package::Element::FUN);
        if(mainFunElP == nullptr){
            throw NotFoundError("no main function found");
        }
        auto assemblyGenerator = AssemblyGenerator{(Fun*)mainFunElP->pointer};
        return assemblyGenerator.compile();
    }

    Parser CompilerFrontend::getParsed(const std::string &filePath) {
        return Parser(Lexer(readFile(filePath)));
    }

    MakePackage &CompilerFrontend::getMakeFile(const std::string &filePath) {
        auto it = makeFiles.find(filePath);
        if(it != makeFiles.end()){
            return *it->second;
        }
        return *makeFiles.emplace(
                filePath,
                new MakePackage(
                        getParsed(filePath),
                        getFileName(filePath),
                        this
                )
            ).first->second;
    }

    std::string CompilerFrontend::getFilePath(Package const*package) {
        if(package->name.empty() || package->name.find_first_not_of(' ') == std::string::npos){
            throw std::runtime_error("no file name");
        }
        std::string path;
        auto pack = package;
        while((pack = (Package*)pack->father.pointer)!=nullptr){
            if(!pack->name.empty())
                path.insert(0,pack->name +"/");
        }
        path += package->name+".felan";
        std::string temp;
        temp = consts.at("FELAN_STDLIB_HOME")+path;
        if(isFileExist(temp)){
            return temp;
        }
        temp = consts.at("SOURCE_HOME")+path;
        if(isFileExist(temp)){
            return temp;
        }
        throw std::runtime_error("no file by path "+path+" exists");
    }

    std::string CompilerFrontend::getFileName(std::string_view filePath) {
        return {std::find(
                    filePath.rbegin(),filePath.rend(),'/'
                ).base(),
                std::find(
                     filePath.rbegin(),filePath.rend(),'.'
                ).base()-1
        };
    }

    std::string CompilerFrontend::readFile(const std::string &filePath) {
        constexpr auto READ_SIZE = std::size_t(4096);
        std::ifstream stream{filePath};
        stream.exceptions(std::ios_base::badbit);
        std::string out,buf(READ_SIZE,'\0');
        while(stream.read(&buf[0],READ_SIZE)){
            out.append(buf,0,stream.gcount());
        }
        out.append(buf,0,stream.gcount());
        return out;
    }

    bool CompilerFrontend::isFileExist(const std::string &filePath) {
        return std::filesystem::exists(filePath);
    }
} // felan
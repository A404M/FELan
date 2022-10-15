//
// Created by a404m on 9/7/22.
//

#include "NativeLibraryLoader.h"
#include <dlfcn.h>

namespace felan {

    NativeLibraryLoader::NativeLibraryLoader() : soHolder(nullptr) {
        //empty
    }

    NativeLibraryLoader::NativeLibraryLoader(const std::string &libPath)
            : soHolder(dlopen(libPath.c_str(),RTLD_NOW)){
        if(!this->soHolder){
            throw std::runtime_error("library " + libPath + " not found");
        }
        try {
            auto fun = getFunction<void (*)()>("init");
            fun();
        }catch(const std::exception &e){

        }
    }

    NativeLibraryLoader::~NativeLibraryLoader() {
        this->close();
    }

    void NativeLibraryLoader::load(const std::string &libPath) {
        if(this->soHolder)
            dlclose(this->soHolder);
        this->soHolder = dlopen(libPath.c_str(),RTLD_NOW);
        if(!this->soHolder){
            throw std::runtime_error("library " + libPath + " not found");
        }
        try {
            auto fun = getFunction<void (*)()>("init");
            fun();
        }catch(const std::exception &e){

        }
    }

    void NativeLibraryLoader::close() {
        if(this->soHolder)
            dlclose(this->soHolder);
        this->soHolder = nullptr;
        for(auto &pair : funCache){
            delete[] pair.second.first->arg_types;
            delete pair.second.first;
        }
        funCache.clear();
    }

    ffi_type *NativeLibraryLoader::stringToFFIType(const std::string &str) {
        if(str == "Int"){
            return &ffi_type_sint32;
        }else if(str == "String"){
            return &ffi_type_pointer;
        }else{
            throw std::runtime_error("todo stringToFFIType");
        }
    }

    void NativeLibraryLoader::callFunction(const std::string &funName, std::string_view funSign,Environment &env) {
        void (*fun)();
        auto argSize = funSign.size()-2;
        void *argValues[argSize];
        std::vector<std::pair<void *,Type>> argValueHolder;
        for(int i = 0;i < argSize;++i){
            auto type = Type(funSign[i]);
            switch(type){
                case BYTE:
                    argValueHolder.push_back({new int8_t((int8_t)env.stackPop()),type});
                    argValues[i] = argValueHolder.back().first;
                    break;
                case UBYTE:
                    argValueHolder.push_back({new uint8_t((uint8_t)env.stackPop()),type});
                    argValues[i] = argValueHolder.back().first;
                    break;
                case SHORT:
                    argValueHolder.push_back({new int16_t((int16_t)env.stackPop()),type});
                    argValues[i] = argValueHolder.back().first;
                    break;
                case USHORT:
                    argValueHolder.push_back({new uint16_t((uint16_t)env.stackPop()),type});
                    argValues[i] = argValueHolder.back().first;
                    break;
                case INT:
                    argValueHolder.push_back({new int32_t((int32_t)env.stackPop()),type});
                    argValues[i] = argValueHolder.back().first;
                    break;
                case UINT:
                    argValueHolder.push_back({new uint32_t((uint32_t)env.stackPop()),type});
                    argValues[i] = argValueHolder.back().first;
                    break;
                case LONG:
                    argValueHolder.push_back({new int64_t((int64_t)env.stackPop()),type});
                    argValues[i] = argValueHolder.back().first;
                    break;
                case ULONG:
                    argValueHolder.push_back({new uint64_t((uint64_t)env.stackPop()),type});
                    argValues[i] = argValueHolder.back().first;
                    break;
                case OBJECT:
                    argValueHolder.emplace_back((void*)env.stackPop(),type);
                    argValues[i] = argValueHolder.back().first;
                    break;
                case VOID:
                default:
                    throw std::runtime_error("bad argument types");
            }
        }
        auto it = funCache.find(funName);
        ffi_cif *cif;
        if(it == funCache.end()){
            cif = new ffi_cif;
            ffi_type *ret = typeToFFIType(Type(funSign.back()));
            auto args = new ffi_type*[argSize];

            fun = (decltype(fun))dlsym(this->soHolder,funName.c_str());
            if(!fun){
                throw std::runtime_error("no function by name of "+funName+" found");
            }

            for(int i = 0;i < argSize;++i){
                args[i] = typeToFFIType(Type(funSign[i]));
            }

            if(ffi_prep_cif(cif,FFI_DEFAULT_ABI,argSize,ret,args) != FFI_OK){
                throw std::runtime_error(funName+" isn't callable");
            }

            funCache.emplace(funName,std::pair(cif,fun));

            //delete[] args;
        }else{
            cif = it->second.first;
            fun = it->second.second;
        }
        ffi_arg rc;
        ffi_call(cif,fun,&rc,argValues);
        env.stackPush((uint64_t)rc);

        for(auto &arg : argValueHolder){
            switch (arg.second) {
                case BYTE:
                    delete (int8_t*)arg.first;
                    break;
                case UBYTE:
                    delete (uint8_t*)arg.first;
                    break;
                case SHORT:
                    delete (int16_t*)arg.first;
                    break;
                case USHORT:
                    delete (uint16_t*)arg.first;
                    break;
                case INT:
                    delete (int32_t*)arg.first;
                    break;
                case UINT:
                    delete (uint32_t*)arg.first;
                    break;
                case LONG:
                    delete (int64_t*)arg.first;
                    break;
                case ULONG:
                    delete (uint64_t*)arg.first;
                    break;
                case OBJECT:
                    //gc will take care of that
                    break;
                case VOID:
                default:
                    throw std::runtime_error("bad argument types");
            }
        }
    }

    template<typename Fun>
    Fun NativeLibraryLoader::getFunction(const std::string &funName) {
        if(!this->soHolder)
            throw std::runtime_error("no library is loaded");
        Fun fun;
        fun = (decltype(fun))dlsym(soHolder,funName.c_str());
        if(!fun){
            throw std::runtime_error("no function by name "+funName);
        }

        return fun;
    }

    ffi_type *NativeLibraryLoader::typeToFFIType(NativeLibraryLoader::Type type) {
        switch (type) {
            case VOID:
                return &ffi_type_void;
            case BYTE:
                return &ffi_type_sint8;
            case UBYTE:
                return &ffi_type_uint8;
            case SHORT:
                return &ffi_type_sint16;
            case USHORT:
                return &ffi_type_uint16;
            case INT:
                return &ffi_type_sint32;
            case UINT:
                return &ffi_type_uint32;
            case LONG:
                return &ffi_type_sint64;
            case ULONG:
                return &ffi_type_uint64;
            case OBJECT:
                return &ffi_type_pointer;
            default:
                throw std::runtime_error("undefined type");
        }
    }
} // felan
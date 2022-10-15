//
// Created by a404m on 9/7/22.
//

#ifndef FELAN_NATIVELIBRARYLOADER_H
#define FELAN_NATIVELIBRARYLOADER_H

#include <string>
#include <vector>
#include <stdexcept>
#include <ffi.h>
#include <map>
#include <felan/native_interface/environment/Environment.h>

namespace felan {

    class NativeLibraryLoader {
    private:
        void *soHolder;
        //map string -> <fun_cif,fun>
        std::map<std::string,std::pair<ffi_cif*,void (*)()>>
                    funCache{};

        //types:
        // v(void) -> only used for return means no return
        // b -> byte or char            (1byte)
        // B -> unsigned byte or char   (1byte)
        // s -> short                   (2byte)
        // S -> unsigned short          (2byte)
        // i -> int                     (4byte)
        // I -> unsigned int            (4byte)
        // l -> long                    (8byte)
        // L -> unsigned long           (8byte)
        // o -> object                  (8byte):pointer
        //[param...]:ret
        enum Type{
            VOID = 'v',
            BYTE = 'b',
            UBYTE = 'B',
            SHORT = 's',
            USHORT = 'S',
            INT = 'i',
            UINT = 'I',
            LONG = 'l',
            ULONG = 'L',
            OBJECT = 'o'
        };

    public:
        NativeLibraryLoader();
        explicit NativeLibraryLoader(const std::string &libPath);
        ~NativeLibraryLoader();

        void load(const std::string &libPath);
        void close();

        static ffi_type *stringToFFIType(const std::string &str);

        void callFunction(const std::string &funName,std::string_view funSign,Environment &env);
    private:

        template<typename Fun>
        Fun getFunction(const std::string &funName);

        ffi_type *typeToFFIType(Type type);
    };

} // felan

#endif //FELAN_NATIVELIBRARYLOADER_H

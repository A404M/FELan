//
// Created by a404m on 9/26/22.
//

#ifndef FELAN_BUILTINFUNCTIONS_H
#define FELAN_BUILTINFUNCTIONS_H

#include <map>
#include <string>
#include <felan/native_interface/environment/Environment.h>
#include <felan/native_interface/felanTypes.h>

namespace felan {
    class Runner;
    class BuiltinFunctions {
    public:
        typedef void (*Function)(Environment &);
        typedef Function FunctionHolder[];
        typedef std::map<std::string,size_t> FunctionToIndex;

        static FunctionHolder functionHolder;
        const static FunctionToIndex functionToIndex;

    private:
        static inline void felan_lang_system_print$felan_lang_primitive_Int$(Environment &env);
        static inline void felan_lang_system_print$felan_lang_string_String$(Environment &env);
        static inline void felan_lang_system_garbageCollector$$(Environment &env);

        static inline void felan_lang_primitive_Int___constructor__$$(Environment &env);
        static inline void felan_lang_primitive_Int___constructor__$felan_lang_primitive_Int$(Environment &env);
        static inline void felan_lang_primitive_Int_toString$$felan_lang_string_String(Environment &env);
        static inline void felan_lang_primitive_Int___positive__$$(Environment &env);
        static inline void felan_lang_primitive_Int___negative__$$(Environment &env);
        static inline void felan_lang_primitive_Int___plus__$felan_lang_primitive_Int$(Environment &env);
        static inline void felan_lang_primitive_Int___minus__$felan_lang_primitive_Int$(Environment &env);
        static inline void felan_lang_primitive_Int___assign__$felan_lang_primitive_Int$(Environment &env);

        static inline void felan_lang_string_String___constructor__$$(Environment &env);
        static inline void felan_lang_string_String___constructor__$__builtin_char_ptr$(Environment &env);
        static inline void felan_lang_string_String___constructor__$felan_lang_string_String$(Environment &env);
        static inline void felan_lang_string_String_toString$$felan_lang_string_String(Environment &env);
        static inline void felan_lang_string_String___plus__$felan_lang_string_String$(Environment &env);
        static inline void felan_lang_string_String___assign__$felan_lang_string_String$(Environment &env);
    };

} // felan

#endif //FELAN_BUILTINFUNCTIONS_H

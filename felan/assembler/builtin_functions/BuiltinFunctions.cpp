//
// Created by a404m on 9/26/22.
//

#include "BuiltinFunctions.h"
#include <felan/vm/runner/Runner.h>
#include <iostream>

namespace felan {

    BuiltinFunctions::FunctionHolder BuiltinFunctions::functionHolder = {
            felan_lang_system_print$felan_lang_primitive_Int$,
            felan_lang_system_print$felan_lang_string_String$,
            felan_lang_system_println$$,
            felan_lang_system_println$felan_lang_primitive_Int$,
            felan_lang_system_println$felan_lang_string_String$,
            felan_lang_system_garbageCollector$$,

            felan_lang_primitive_Int___constructor__$$,
            felan_lang_primitive_Int___constructor__$felan_lang_primitive_Int$,
            felan_lang_primitive_Int_toString$$felan_lang_string_String,
            felan_lang_primitive_Int___positive__$$,
            felan_lang_primitive_Int___negative__$$,
            felan_lang_primitive_Int___plus__$felan_lang_primitive_Int$,
            felan_lang_primitive_Int___minus__$felan_lang_primitive_Int$,
            felan_lang_primitive_Int___assign__$felan_lang_primitive_Int$,

            felan_lang_string_String___constructor__$$,
            felan_lang_string_String___constructor__$__builtin_char_ptr$,
            felan_lang_string_String___constructor__$felan_lang_string_String$,
            felan_lang_string_String_toString$$felan_lang_string_String,
            felan_lang_string_String___plus__$felan_lang_string_String$,
            felan_lang_string_String___assign__$felan_lang_string_String$,
    };
    BuiltinFunctions::FunctionToIndex::value_type::second_type secondIndexInit = -1;
    const BuiltinFunctions::FunctionToIndex BuiltinFunctions::functionToIndex = {
            {"felan.lang.system.print(felan.lang.primitive.Int)",++secondIndexInit},
            {"felan.lang.system.print(felan.lang.string.String)",++secondIndexInit},
            {"felan.lang.system.println()",++secondIndexInit},
            {"felan.lang.system.println(felan.lang.primitive.Int)",++secondIndexInit},
            {"felan.lang.system.println(felan.lang.string.String)",++secondIndexInit},
            {"felan.lang.system.garbageCollector()",++secondIndexInit},

            {"felan.lang.primitive.Int.__constructor__()",++secondIndexInit},
            {"felan.lang.primitive.Int.__constructor__(felan.lang.primitive.Int)",++secondIndexInit},
            {"felan.lang.primitive.Int.toString():felan.lang.string.String",++secondIndexInit},
            {"felan.lang.primitive.Int.__positive__():felan.lang.primitive.Int",++secondIndexInit},
            {"felan.lang.primitive.Int.__negative__():felan.lang.primitive.Int",++secondIndexInit},
            {"felan.lang.primitive.Int.__plus__(felan.lang.primitive.Int):felan.lang.primitive.Int",++secondIndexInit},
            {"felan.lang.primitive.Int.__minus__(felan.lang.primitive.Int):felan.lang.primitive.Int",++secondIndexInit},
            {"felan.lang.primitive.Int.__assign__(felan.lang.primitive.Int):felan.lang.primitive.Int",++secondIndexInit},

            {"felan.lang.string.String.__constructor__()",++secondIndexInit},
            {"felan.lang.string.String.__constructor__(__builtin_char_ptr)",++secondIndexInit},
            {"felan.lang.string.String.__constructor__(felan.lang.string.String)",++secondIndexInit},
            {"felan.lang.string.String.toString():felan.lang.string.String",++secondIndexInit},
            {"felan.lang.string.String.__plus__(felan.lang.string.String):felan.lang.string.String",++secondIndexInit},
            {"felan.lang.string.String.__assign__(felan.lang.string.String):felan.lang.string.String",++secondIndexInit},
    };

    void BuiltinFunctions::felan_lang_system_print$felan_lang_primitive_Int$(Environment &env){
        std::cout << *(uint64_t*)env.stackPop();
    }

    void BuiltinFunctions::felan_lang_system_print$felan_lang_string_String$(Environment &env) {
        ((felan_lang_string_String*)env.stackPop())->print();
    }

    void BuiltinFunctions::felan_lang_system_println$$(Environment &env){
        std::cout << std::endl;
    }

    void BuiltinFunctions::felan_lang_system_println$felan_lang_primitive_Int$(Environment &env){
        std::cout << *(uint64_t*)env.stackPop() << std::endl;
    }

    void BuiltinFunctions::felan_lang_system_println$felan_lang_string_String$(Environment &env) {
        ((felan_lang_string_String*)env.stackPop())->print();
        std::cout << std::endl;
    }

    void BuiltinFunctions::felan_lang_system_garbageCollector$$(Environment &env) {
        env.garbageCollector();
    }

    void BuiltinFunctions::felan_lang_primitive_Int___constructor__$$(Environment &env) {
        env.stackPush(
                (uint64_t)
                (env.makeNewObjectEmplaceT<uint64_t>(0))
            );
    }

    void BuiltinFunctions::felan_lang_primitive_Int___constructor__$felan_lang_primitive_Int$(Environment &env) {
        auto i = *(uint64_t*)env.stackPop();
        env.stackPush(
                (uint64_t)
                (env.makeNewObjectEmplaceT<uint64_t>(i))
            );
    }

    void BuiltinFunctions::felan_lang_primitive_Int_toString$$felan_lang_string_String(Environment &env) {
        auto i = *(uint64_t*)env.stackPop();
        env.stackPush(
                (uint64_t)
                (env.makeNewObjectEmplaceT<felan_lang_string_String>(env,std::to_string(i).c_str()))
            );
    }

    void BuiltinFunctions::felan_lang_primitive_Int___positive__$$(Environment &env) {
        //empty
    }

    void BuiltinFunctions::felan_lang_primitive_Int___negative__$$(Environment &env) {
        *(uint64_t*)env.stackTop() =
                -*(uint64_t*)env.stackTop();
    }

    void BuiltinFunctions::felan_lang_primitive_Int___plus__$felan_lang_primitive_Int$(Environment &env) {
        auto a = *(int64_t*)env.stackPop();
        *(uint64_t*)env.stackTop() += a;
    }

    void BuiltinFunctions::felan_lang_primitive_Int___minus__$felan_lang_primitive_Int$(Environment &env) {
        auto a = *(int64_t*)env.stackPop();
        *(uint64_t*)env.stackTop() -= a;
    }

    void BuiltinFunctions::felan_lang_primitive_Int___assign__$felan_lang_primitive_Int$(Environment &env) {
        auto a = *(int64_t*)env.stackPop();
        *(uint64_t*)env.stackTop() = a;
    }

    void BuiltinFunctions::felan_lang_string_String___constructor__$$(Environment &env) {
        env.stackPush((uint64_t)env.makeNewObjectEmplaceT<felan_lang_string_String>());
    }

    void BuiltinFunctions::felan_lang_string_String___constructor__$__builtin_char_ptr$(Environment &env) {
        auto cstr = (char*)env.stackPop();
        env.stackPush((uint64_t)env
            .makeNewObjectEmplaceT<felan_lang_string_String>(
                env,cstr
                )
            );
    }

    void BuiltinFunctions::felan_lang_string_String___constructor__$felan_lang_string_String$(Environment &env) {
        env.stackPush(
                (uint64_t)env.makeNewObjectEmplaceT<felan_lang_string_String>(
                        env,
                        *(felan_lang_string_String*)
                        env.stackPop()
                    )
                );
    }

    void BuiltinFunctions::felan_lang_string_String_toString$$felan_lang_string_String(Environment &env) {
        //empty
    }

    void BuiltinFunctions::felan_lang_string_String___plus__$felan_lang_string_String$(Environment &env) {
        auto pstr1 = (felan_lang_string_String*)(env.stackPop());
        auto pstr2 = (felan_lang_string_String*)(env.stackPop());
        auto result = env.makeNewObjectEmplaceT<felan_lang_string_String>(
                        env,
                        pstr1->begin,pstr1->size()
                );
        result->append(env,*pstr2);
        env.stackPush((uint64_t)result);
    }

    void BuiltinFunctions::felan_lang_string_String___assign__$felan_lang_string_String$(Environment &env) {
        auto pstr1 = (felan_lang_string_String*)(env.stackPop());
        auto pstr2 = (felan_lang_string_String*)(env.stackPop());
        pstr1->assign(env,*pstr2);
        env.stackPush((uint64_t)pstr2);
    }


} // felan
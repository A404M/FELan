cmake_minimum_required(VERSION 3.23)
project(FELan)

set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_STANDARD 23)
if(1)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")
else()
    set(CMAKE_CXX_FLAGS "-g")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()
#set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(PRIVATE .)

set(FELAN_COMMON_FILES
        felan/common/node/Node.cpp
        felan/common/node/Node.h
        felan/common/lexer/Lexer.cpp
        felan/common/lexer/Lexer.h
        felan/common/parser/Parser.cpp
        felan/common/parser/Parser.h
        felan/common/variable/Variable.cpp
        felan/common/variable/Variable.h
        felan/common/class/Class.cpp
        felan/common/class/Class.h
        felan/common/fun/Fun.cpp
        felan/common/fun/Fun.h
        felan/common/package/Package.cpp
        felan/common/package/Package.h
        felan/common/expression/Expression.cpp
        felan/common/expression/Expression.h
        felan/common/parent/Parent.cpp
        felan/common/parent/Parent.h
        felan/common/error/syntax_error/SyntaxError.cpp felan/common/error/syntax_error/SyntaxError.h felan/common/error/not_found_error/NotFoundError.cpp felan/common/error/not_found_error/NotFoundError.h felan/common/attribute/Attribute.cpp felan/common/attribute/Attribute.h)

set(FELAN_NATIVE_INTERFACE
        felan/native_interface/environment/Environment.h
        felan/native_interface/environment/Environment.cpp
        felan/native_interface/felanTypes.h
        )

set(FELAN_COMMON_INTERFACE
        felan/native_library_loader/NativeLibraryLoader.cpp
        felan/native_library_loader/NativeLibraryLoader.h)

set(FELAN_COMPILER_FILES
        felan/compiler/make_package/MakePackage.cpp
        felan/compiler/make_package/MakePackage.h
        felan/compiler/compiler_frontend/CompilerFrontend.cpp
        felan/compiler/compiler_frontend/CompilerFrontend.h
        felan/compiler/assembly_generator/AssemblyGenerator.cpp felan/compiler/assembly_generator/AssemblyGenerator.h)

set(FELAN_ASSEMBLER_FILES
        felan/assembler/bytecode_assembler/BytecodeAssembler.cpp
        felan/assembler/bytecode_assembler/BytecodeAssembler.h
        felan/assembler/instruction/Instruction.cpp
        felan/assembler/instruction/Instruction.h
        felan/assembler/assembler_lexer/AssemblerLexer.cpp
        felan/assembler/assembler_lexer/AssemblerLexer.h
        felan/assembler/builtin_functions/BuiltinFunctions.cpp felan/assembler/builtin_functions/BuiltinFunctions.h)

set(FELAN_VM_FILES
        felan/vm/runner/Runner.cpp
        felan/vm/runner/Runner.h
        felan/vm/stack/Stack.h
        )

add_library(felan_lang_primitives SHARED
        felan_project/felan_stdlib_native/felan/lang/primitives.cpp
        )

link_directories(/usr/lib)

if(1)
    add_compile_definitions(FELAN_COMPILE)
    set(FELAN_TO_COMPILE_FILES
            ${FELAN_COMMON_FILES}
            ${FELAN_COMMON_INTERFACE}
            ${FELAN_NATIVE_INTERFACE}
            #${FELAN_INTERPRETER_FILES}
            ${FELAN_COMPILER_FILES}
            ${FELAN_ASSEMBLER_FILES}
            ${FELAN_VM_FILES}
            )
else()
    add_compile_definitions(FELAN_ASSEMBLE)
    set(FELAN_TO_COMPILE_FILES
            #${FELAN_COMMON_FILES}
            ${FELAN_COMMON_INTERFACE}
            ${FELAN_NATIVE_INTERFACE}
            #${FELAN_INTERPRETER_FILES}
            #${FELAN_COMPILER_FILES}
            ${FELAN_ASSEMBLER_FILES}
            ${FELAN_VM_FILES}
            )
endif()
add_executable(FELan
        felan/main.cpp
        ${FELAN_TO_COMPILE_FILES}
        )

target_link_libraries(FELan ffi)

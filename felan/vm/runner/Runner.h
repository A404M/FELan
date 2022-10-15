//
// Created by a404m on 9/24/22.
//

#ifndef FVM_RUNNER_H
#define FVM_RUNNER_H

#include <felan/assembler/bytecode_assembler/BytecodeAssembler.h>
#include <felan/vm/stack/Stack.h>
#include <felan/native_interface/environment/Environment.h>
#include <felan/native_library_loader/NativeLibraryLoader.h>
#include <list>

namespace felan {

    class Runner {
        friend class BuiltinFunctions;
        friend struct Environment;
    private:
        template<typename T>
        using Stack = Stack<T>;
        typedef BytecodeAssembler::BytecodeHolder BytecodeHolder;
        typedef uint64_t* VariablePointer;
        typedef uint8_t* CodePointer;
        typedef uint8_t* DataPointer;
        typedef void* Object;
        typedef std::vector<Object> VariableHolder;
        typedef std::vector<std::pair<uint8_t*,uint64_t>> RawObjectPointerHolder;
        typedef std::map<std::string,NativeLibraryLoader> NativeLibraryLoaderHolder;

        const BytecodeHolder bytecodeHolder;
        const VariablePointer variablePointer = nullptr;
        const CodePointer codePointer = nullptr;
        const DataPointer dataPointer = nullptr;
        Stack<uint64_t> stack{};
        Stack<VariableHolder> funVariablesStack{};
        Stack<uint64_t> callStack{};
        RawObjectPointerHolder rawObjectPointerHolder{};
        Environment env;
        NativeLibraryLoaderHolder nativeLibraryLoaderHolder{};

    public:
        explicit Runner(BytecodeHolder _bytecodeHolder);
        ~Runner();

        void run();
    private:
        void callNative();

        template<typename T>
        static T &derefAs(void *p);

        template<typename T>
        static bool isInRange(T begin,T end,T v);

        void *makeNewObject(uint64_t size);
        void garbageCollector();

        inline bool hasInStack(uint64_t value);
        inline bool hasInVariable(Object object);

        template<typename T>
        static bool hasPointerTo(uint8_t *begin,const uint8_t *end,T t);
    };

} // felan

#endif //FVM_RUNNER_H

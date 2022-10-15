#include <iostream>
#include <chrono>

//#define FELAN_COMPILE
//#define FELAN_ASSEMBLE
//#define FELAN_GET_FILE_INPUT
#define FELAN_TESTING

#ifdef FELAN_GET_FILE_INPUT
#include <fstream>
#endif

#ifdef FELAN_COMPILE
#include <felan/compiler/compiler_frontend/CompilerFrontend.h>
void compile(std::string_view code);
#endif
//#ifdef FELAN_ASSEMBLE
#include <felan/vm/runner/Runner.h>
void assemble(std::string_view code);
//#endif

int main(
#ifdef FELAN_GET_FILE_INPUT
        int argc,char *argv[]
#endif
        ) {
#ifdef FELAN_TESTING
    for(int i = 0;i < 10;i++) {
#endif
#ifdef FELAN_GET_FILE_INPUT
        if(argc < 2)
            throw std::runtime_error("no file name input found");
        std::ifstream file{argv[1]};
        if(!file){
            throw std::runtime_error("no file found");
        }
        std::string code{std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>()};
#else
        std::string code =
#ifdef FELAN_COMPILE
        "/home/a404m/ProgrammingProjects/CLionProjects/FELan/felan_project/";
#endif
#ifdef FELAN_ASSEMBLE
        R"(
.data
%1 1
.code
#main.main()
PUSH 1
ALLOC_NVAR
CALL_BUILTIN felan.lang.primitive.Int.__constructor__()
OPOP 0
SPUSH %1
OPOP 0
OPUSH 0
CALL_BUILTIN felan.lang.system.print(felan.lang.primitive.Int)
RET

)"
#endif
;
#endif
#ifdef FELAN_COMPILE
        compile(code);
#endif
#ifdef FELAN_ASSEMBLE
        assemble(code);
#endif
#ifdef FELAN_TESTING
    }
#endif
    return 0;
}
#ifdef FELAN_COMPILE
void compile(std::string_view projectDir){
    auto start = std::chrono::high_resolution_clock::now();
    felan::CompilerFrontend compilerFrontend{projectDir};
    assemble(compilerFrontend.compile());
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration<double,std::milli>(end-start).count();
    auto total = diff;
    //std::cout << "compile:\t\t" << diff << "ms" << std::endl;
    std::cout << "total:\t\t\t" << total << "ms" << std::endl;
    felan::MakePackage::clearRootPackage();
    std::cout << "-------------------------------------------" << std::endl;
}
#endif

//#ifdef FELAN_ASSEMBLE
void assemble(std::string_view code){
    auto ba = felan::BytecodeAssembler(
            felan::AssemblerLexer::simpleLex(code)
            );
    auto runner = felan::Runner(ba.compile());
    std::cout << "------program started\n";
    runner.run();
    std::cout << "\n------program ended\n";
}
//#endif

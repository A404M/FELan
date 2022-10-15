//
// Created by a404m on 9/27/22.
//

#include "Environment.h"
#include <felan/vm/runner/Runner.h>

namespace felan{
    Environment::Environment(Runner *_runner) : runner(_runner) {
        //empty
    }

    void *Environment::makeNewObject(uint64_t size) {
        return runner->makeNewObject(size);
    }

    void Environment::stackPush(uint64_t v) {
        runner->stack.push(v);
    }

    uint64_t &Environment::stackTop() {
        return runner->stack.top();
    }

    uint64_t Environment::stackPop() {
        return runner->stack.getPop();
    }

    void Environment::garbageCollector() {
        runner->garbageCollector();
    }
}//felan
//
// Created by a404m on 9/9/22.
//

#ifndef FELAN_ENVIRONMENT_H
#define FELAN_ENVIRONMENT_H

#include <felan/vm/stack/Stack.h>
#include <cstring>

namespace felan {

    class Runner;
    struct Environment {
        friend class Runner;
    private:
        Runner *runner;
    public:
        explicit Environment(Runner *_runner);

        void *makeNewObject(uint64_t size);
        void stackPush(uint64_t v);
        uint64_t &stackTop();
        uint64_t stackPop();

        void garbageCollector();

        template<typename T,typename ...Args>
        T *makeNewObjectEmplaceT(Args&& ...args){
            auto p = static_cast<T*>(this->makeNewObject(sizeof(T)));
            *p = std::move(T(args...));
            return p;
        }

        template<typename T>
        T *makeNewObjectT(T t){
            auto p = static_cast<T*>(this->makeNewObject(sizeof(T)));
            *p = t;
            return p;
        }

        template<typename T>
        static T &as(void *p){
            return *static_cast<T*>(p);
        }
    };

} // felan

#endif //FELAN_ENVIRONMENT_H

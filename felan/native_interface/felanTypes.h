//
// Created by a404m on 9/11/22.
//

#ifndef FELAN_FELANTYPES_H
#define FELAN_FELANTYPES_H

#include <felan/native_interface/environment/Environment.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <syscall.h>

namespace felan {
    struct felan_lang_string_String {
        typedef uint64_t Size;

        char *begin;
        char *end;
        char *reserved;

        felan_lang_string_String(const felan_lang_string_String &) = delete;
        felan_lang_string_String(felan_lang_string_String &&) = default;

        felan_lang_string_String() : begin(nullptr), end(nullptr), reserved(nullptr){
            //empty
        }

        felan_lang_string_String(Environment &env, const char* cstr, uint64_t len){
            auto reserveLen = len+len/2+1;
            this->begin = static_cast<char *>(env.makeNewObject(reserveLen));
            this->end = this->begin+len;
            this->reserved = this->begin+reserveLen;
            std::strcpy(this->begin,cstr);
        }

        felan_lang_string_String(felan::Environment &env, const char *cstr) {
            auto len = std::strlen(cstr);
            auto reserveLen = len + len / 2 + 1;
            begin = (char *) env.makeNewObject(reserveLen);
            std::strcpy(begin, cstr);
            end = begin+len;
            reserved = begin+reserveLen;
        }

        felan_lang_string_String(Environment &env,const felan_lang_string_String &str) : begin((char*)env.makeNewObject(str.capacity())),
        end(this->begin+str.size()), reserved(this->begin+str.capacity()){
            std::strcpy(this->begin,str.begin);
        }

        felan_lang_string_String &operator=(felan_lang_string_String &&) = default;

        [[nodiscard]] Size capacity() const {
            return this->reserved - this->begin;
        }

        [[nodiscard]] Size size() const {
            return this->end - this->begin;
        }

        void resizeAndClear(Environment &env,uint64_t sz) {
            this->begin = (char*)env.makeNewObject(sz);
            this->end = this->begin;
            this->reserved = this->begin+sz;
        }

        void append(felan::Environment &env, const felan_lang_string_String &str) {
            auto newSize = this->size() + str.size();
            if (this->capacity() - this->size() <= str.size()) {
                this->end = this->begin;
                auto len = newSize + newSize / 2 + 1;
                this->begin = static_cast<char *>(env.makeNewObject(len));
                this->reserved = this->begin + len;
                std::strcpy(this->begin, this->end);
            }
            std::strcat(this->begin, str.begin);
            this->end = this->begin + newSize;
        }

        void print() const{
            syscall(SYS_write,STDOUT_FILENO,begin,end-begin);
        }

        felan_lang_string_String &assign(Environment &env,const felan_lang_string_String &str){
            if(this->capacity() < str.size()){
                this->resizeAndClear(env,str.size()+str.size()/2+1);
            }
            strcpy(this->begin,str.begin);
            return *this;
        }

        [[nodiscard]] std::string_view toStringView() const{
            return {this->begin,this->end};
        }

        [[nodiscard]] std::string toString() const{
            return {this->begin,this->end};
        }
    };
}

#endif //FELAN_FELANTYPES_H

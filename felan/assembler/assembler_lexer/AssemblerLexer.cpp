//
// Created by a404m on 9/23/22.
//

#include "AssemblerLexer.h"
#include <cmath>
#include <algorithm>

namespace felan {
    AssemblerLexer::Holder AssemblerLexer::simpleLex(std::string_view code) {
        Holder holder{{}};
        auto itHolder = holder.begin();
        std::string str;

        /*
         * strings start with " or '
         * numbers start with a digit or - or +
         * labels start with #
         */
        for(auto it = code.begin(),end = code.end();it < end;++it){
            auto current = *it;
            if(current == ';'){
                it = std::find(it+1,end,'\n');
                goto NEWLINE;
            }else if(current == '\n'){
                NEWLINE:
                push_clear_ifn_empty(itHolder,str);
                create_new_line(holder,itHolder);
            }else if(std::isspace(current)){
                push_clear_ifn_empty(itHolder,str);
            }else if(current == '\"' || current == '\''){
                push_clear_ifn_empty(itHolder,str);
                str += current;
                for (++it;; ++it) {
                    char c = *it;
                    if(it >= end){
                        throw std::runtime_error(std::string("no closing ")+c);
                    }
                    if(c == current){//end
                        //str += c;
                        break;
                    }else if (c == '\\') {
                        switch (*++it) {
                            case 'a':
                                c = '\a';
                                break;
                            case 'b':
                                c = '\b';
                                break;
                            case 'f':
                                c = '\f';
                                break;
                            case 'n':
                                c = '\n';
                                break;
                            case 'r':
                                c = '\r';
                                break;
                            case 't':
                                c = '\t';
                                break;
                            case 'v':
                                c = '\v';
                                break;
                            case '\'':
                                c = '\'';
                                break;
                            case '\"':
                                c = '\"';
                                break;
                            case '\\':
                                //c = '\\';//is is already
                                break;
                            case 'u':
                                str += hexToUTF8(code.substr(it - code.cbegin(), 6));
                                continue;
                            default:
                                throw std::runtime_error(std::string("unknown \\") + *it);
                        }
                    }
                    str += c;
                }
                push_clear_ifn_empty(itHolder,str);
            }else{
                str += current;
            }
        }
        push_clear_ifn_empty(itHolder,str);
        if(itHolder->empty())
            holder.erase(itHolder);

        return holder;
    }

    void AssemblerLexer::push_clear(Holder::iterator itHolder, std::string &str) {
        std::swap(itHolder->emplace_back(),str);
    }

    void AssemblerLexer::push_clear_ifn_empty(Holder::iterator itHolder, std::string &str) {
        if(!str.empty())
            push_clear(itHolder,str);
    }

    void AssemblerLexer::create_new_line(Holder &holder,Holder::iterator &itHolder) {
        if(!itHolder->empty()){
            holder.emplace_back();
            itHolder = holder.end()-1;
        }
    }

    uint64_t AssemblerLexer::hexToInt(std::string_view hex) {
        uint64_t result = 0;
        for (const auto &c: hex) {
            result <<= 4;
            char x = (char) tolower(c);
            if (x >= '0' && x <= '9') {
                result |= (x - '0');
            } else if (x >= 'a' && x <= 'f') {
                result |= x - ('a' - 10);
            } else {
                throw std::runtime_error(c + std::string(" is not a hex value"));
            }
        }
        return result;
    }

    std::string AssemblerLexer::hexToUTF8(std::string_view hex) {
        uint64_t val = hexToInt(hex);
        std::string result;
        if (val <= 128) {
            result = (char) val;
        } else {
            auto i = val;
            while (val) {
                i &= 0xFFFF'FFFF'FFFF'FFC0;
                auto c = (unsigned char) (val ^ i);
                val >>= 6;
                i >>= 6;
                if (!val) {
                    if (result.size() > 6) {
                        throw std::runtime_error(std::string(hex) + " unicode number is too big");
                    } else if (c < std::pow(2, 6 - result.size())) {
                        unsigned char flag = 0;
                        const int count = (int)result.size();
                        for (int j = 0; j <= count; j++) {
                            flag >>= 1;
                            flag |= 0b1000'0000;
                        }
                        c |= flag;
                    }
                } else {
                    c |= 0b1000'0000;
                }
                result.insert(result.begin(), (char) c);
            }
        }
        return result;
    }
} // felan
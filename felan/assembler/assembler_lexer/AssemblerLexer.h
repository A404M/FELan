//
// Created by a404m on 9/23/22.
//

#ifndef FVM_ASSEMBLERLEXER_H
#define FVM_ASSEMBLERLEXER_H

#include <string>
#include <vector>
#include <stdexcept>

namespace felan {

    class AssemblerLexer {
    public:
        typedef std::vector<std::string> Line;
        typedef std::vector<Line> Holder;
        static Holder simpleLex(std::string_view code);

        AssemblerLexer() = delete;
        AssemblerLexer(const AssemblerLexer &) = delete;

        AssemblerLexer &operator=(const AssemblerLexer&) = delete;

    private:
        static inline void push_clear(Holder::iterator itHolder,std::string &str);
        static inline void push_clear_ifn_empty(Holder::iterator itHolder,std::string &str);
        static inline void create_new_line(Holder &holder,Holder::iterator &itHolder);

        static uint64_t hexToInt(std::string_view hex);
        static std::string hexToUTF8(std::string_view hex);
    };

} // felan

#endif //FVM_ASSEMBLERLEXER_H

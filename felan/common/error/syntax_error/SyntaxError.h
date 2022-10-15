//
// Created by a404m on 10/12/22.
//

#ifndef FELAN_SYNTAXERROR_H
#define FELAN_SYNTAXERROR_H

#include <stdexcept>

namespace felan {

    class SyntaxError : public std::runtime_error {
    public:
        explicit SyntaxError(const std::string& str) : std::runtime_error(str){

        }

        std::string what(){
            return ((std::runtime_error*)this)->what();
        }
    };

} // felan

#endif //FELAN_SYNTAXERROR_H

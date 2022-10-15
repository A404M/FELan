//
// Created by a404m on 10/12/22.
//

#ifndef FELAN_NOTFOUNDERROR_H
#define FELAN_NOTFOUNDERROR_H

#include <stdexcept>

namespace felan {

    class NotFoundError : public std::runtime_error{
    public:
        explicit NotFoundError(const std::string &str) : std::runtime_error(str){
            //empty
        }

        std::string what(){
            return ((std::runtime_error*)this)->what();
        }
    };

} // felan

#endif //FELAN_NOTFOUNDERROR_H

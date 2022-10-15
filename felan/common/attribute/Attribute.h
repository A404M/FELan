//
// Created by a404m on 10/13/22.
//

#ifndef FELAN_ATTRIBUTE_H
#define FELAN_ATTRIBUTE_H

#include <cstdint>
#include <felan/common/node/Node.h>
#include <felan/common/error/syntax_error/SyntaxError.h>

namespace felan {

    class Attribute {
    public:
        typedef std::uint16_t AttributeType;
        enum AttributeEnum : AttributeType {
            ATT_NONE =      0,
            ATT_PUBLIC =    0b1,
            ATT_PRIVATE =   0b10,
            ATT_PROTECT =   0b100,
            ATT_OPEN =      0b1000,
            ATT_CLOSE =     0b10000,
            ATT_NATIVE =    0b100000,
            ATT_BUILTIN =   0b1000000,
            ATT_OVERRIDE =  0b10000000,
            ATT_OPERATOR =  0b100000000,
            ATT_STATIC =    0b1000000000
        };
        constexpr static auto ATTRIBUTE_DEFAULT = (AttributeEnum)(ATT_PUBLIC | ATT_OPEN);

        AttributeEnum attributeEnum = ATT_NONE;
        std::string native;

        explicit Attribute(Node &node);
        Attribute() = default;

        bool isAttributeWrong() const;

        bool has(AttributeEnum att) const;

        static AttributeEnum stringToAttributeEnum(const std::string &str);

        static inline bool has(AttributeEnum att1,AttributeEnum att2);
        static inline void put(AttributeEnum &att1,AttributeEnum att2);
    };

} // felan

#endif //FELAN_ATTRIBUTE_H

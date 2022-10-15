//
// Created by a404m on 10/13/22.
//

#include "Attribute.h"

namespace felan {
    Attribute::Attribute(Node &node) : attributeEnum(), native() {
        auto it = node.operands.begin(),end = node.operands.end();
        //parse
        for(;it < end;++it){
            auto att = stringToAttributeEnum(it->str);
            if(att == ATT_NONE){
                break;
            }
            if(att == ATT_NATIVE){
                ++it;
                if(it->token == Node::T_STR){
                    this->native = it->str;
                }
            }
            put(this->attributeEnum,att);
        }
        node.operands.erase(node.operands.begin(),it);

        if(this->isAttributeWrong()){
            throw SyntaxError("attribute is wrong");
        }

        if(attributeEnum == ATT_NONE){
            attributeEnum = ATTRIBUTE_DEFAULT;
        }
    }

    bool Attribute::isAttributeWrong() const {
        return (has(this->attributeEnum,ATT_PUBLIC) && (has(this->attributeEnum,ATT_PRIVATE) || has(this->attributeEnum,ATT_PROTECT)))
            ||   (has(this->attributeEnum,ATT_OPEN) && has(this->attributeEnum,ATT_CLOSE));
    }

    bool Attribute::has(Attribute::AttributeEnum att) const {
        return has(this->attributeEnum,att);
    }

    Attribute::AttributeEnum Attribute::stringToAttributeEnum(const std::string &str) {
        if(str == "public"){
            return ATT_PUBLIC;
        }else if(str == "private"){
            return ATT_PRIVATE;
        }else if(str == "protect"){
            return ATT_PROTECT;
        }else if(str == "open"){
            return ATT_OPEN;
        }else if(str == "close"){
            return ATT_CLOSE;
        }else if(str == "native"){
            return ATT_NATIVE;
        }else if(str == "builtin"){
            return ATT_BUILTIN;
        }else if(str == "override"){
            return ATT_OVERRIDE;
        }else if(str == "operator"){
            return ATT_OPERATOR;
        }else if(str == "static") {
            return ATT_STATIC;
        }else{
            return ATT_NONE;
        }
    }

    bool Attribute::has(Attribute::AttributeEnum att1, Attribute::AttributeEnum att2) {
        return att1&att2;
    }

    void Attribute::put(Attribute::AttributeEnum &att1, Attribute::AttributeEnum att2) {
        if(!has(att1,att2)){
            ((AttributeType &)att1) |= (AttributeType)att2;
        }
    }
} // felan
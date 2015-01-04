//
//  olvalue.h
//  ol-cpp
//
//  Created by 伍 威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#ifndef __ol_cpp__olvalue__
#define __ol_cpp__olvalue__

#include <vector>
#include <map>
#include <string>

namespace OL {
    
    class Value {
    public:
        enum ValueType {
            Null, Number, String, Bool, Array, Object, Path, List, Negative, Quote
        };
        Value();
        ~Value();
        std::string description();
        friend class JSON;
        Value(const Value& value) = delete;
        Value(Value&& value);
    private:
        ValueType _type;
        std::string descriptionNull();
        std::string descriptionNumber();
        std::string descriptionString();
        std::string descriptionBool();
        std::string descriptionArray();
        std::string descriptionObject();
        std::string descriptionPath();
        std::string descriptionList();
        std::string descriptionNegative();
        std::string descriptionQuote();
        union {
            double _number;
            bool _bool;
            std::string* _string;
            std::vector<Value>* _array;
            std::map<std::string, Value>* _object;
            std::pair<Value, std::vector<Value>>* _path;
            Value* _value;
        };
    };
}
#endif /* defined(__ol_cpp__olvalue__) */
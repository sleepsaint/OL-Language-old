//
//  oljson.cpp
//  ol-cpp
//
//  Created by 伍 威 on 14/12/31.
//  Copyright (c) 2014年 sleepsaint. All rights reserved.
//

#include "oljson.h"
#include <iostream>
using namespace std;

namespace OL {
    
    JSON::JSON(char* source, size_t length)
    : _source(source), _end(source + length), _cursor(source), _token(0) {
        nextToken();
    }
    
    void JSON::nextToken() {
        const char* start;
        if (_cursor < _end) {
            auto c = *_cursor;
            switch (c) {
                case '{':
                case '}':
                case '[':
                case ']':
                case ',':
                case ':':
                    _token = c;
                    ++_cursor;
                    break;
                case '"':
                    _token = STRING_TOKEN;
                    ++_cursor;
                    _tokenString = _cursor;
                    _tokenStringEnd = _cursor;
                    unescape();
                    break;
                case 'n':
                    _token = NULL_TOKEN;
                    _cursor += 4;
                    break;
                case 't':
                    _token = BOOL_TOKEN;
                    _cursor += 4;
                    _tokenBool = true;
                    break;
                case 'f':
                    _token = BOOL_TOKEN;
                    _cursor += 5;
                    _tokenBool = false;
                    break;
                case '\n':
                case ' ':
                    ++_cursor;
                    nextToken();
                    break;
                default:
                    _token = NUMBER_TOKEN;
                    start = _cursor;
                    while (_cursor < _end) {
                        switch (*_cursor) {
                            case '{':
                            case '}':
                            case '[':
                            case ']':
                            case ',':
                                goto PP;
                            default:
                                ++_cursor;
                        }
                    }
                PP: _tokenNumber = atof(start);
            }
        } else {
            _token = 0;
        }
    }
    
    bool JSON::getNumber(Value& value) {
        if (match(NUMBER_TOKEN)) {
            value._type = Value::Number;
            value._number = _tokenNumber;
            return true;
        }
        return false;
    }
    
    bool JSON::getString(Value& value) {
        if (match(STRING_TOKEN)) {
            value._type = Value::String;
            value._string = new string(_tokenString, _tokenStringEnd - _tokenString);
            return true;
        }
        return false;
    }
    
    bool JSON::getBool(Value& value) {
        if (match(BOOL_TOKEN)) {
            value._type = Value::Bool;
            value._bool = _tokenBool;
            return true;
        }
        return false;
    }
    
    bool JSON::getNull(Value& value) {
        if (match(NULL_TOKEN)) {
            return true;
        }
        return false;
    }
    
    bool JSON::getObject(Value& value) {
        if (match('{')) {
            value._type = Value::Object;
            value._object = new map<string, Value>;
            if (getPair(value)) {
                while (match(',')) {
                    if (!getPair(value)) {
                        return false;
                    }
                }
            }
            if (match('}')) {
                return true;
            }
        }
        return false;
    }
    
    bool JSON::getArray(Value& value) {
        if (match('[')) {
            value._type = Value::Array;
            value._array = new vector<Value>;
            if (getElement(value)) {
                while (match(',')) {
                    if (!getElement(value)) {
                        return false;
                    }
                }
            }
            if (match(']')) {
                return true;
            }
        }
        return false;
    }
    
    bool JSON::getPair(Value &object) {
        auto start = _tokenString;
        auto end = _tokenStringEnd;
        if (match(STRING_TOKEN) && match(':')) {
            Value value;
            if (getValue(value)) {
                object._object->insert(pair<string, Value>(string(start, end - start), move(value)));
                return true;
            }
        }
        return false;
    }
    
    bool JSON::getElement(Value& array) {
        Value value;
        if (getValue(value)) {
            array._array->push_back(move(value));
            return true;
        }
        return false;
    }
    
    inline int char_to_num(char c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        } else if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            return c - 'A' + 10;
        } else {
            return 0;
        }
    }
    
    void JSON::unescape() {
        while (_cursor < _end) {
            auto c = *_cursor++;
            if (c == '\\') {
                auto d = *_cursor++;
                switch (d) {
                    case '"':
                        *_tokenStringEnd = '"';
                        ++_tokenStringEnd;
                        break;
                    case '\\':
                        *_tokenStringEnd  = '\\';
                        ++_tokenStringEnd;
                        break;
                    case '/':
                        *_tokenStringEnd = '/';
                        ++_tokenStringEnd;
                        break;
                    case 'b':
                        *_tokenStringEnd = '\b';
                        ++_tokenStringEnd;
                        break;
                    case 'f':
                        *_tokenStringEnd = '\f';
                        ++_tokenStringEnd;
                        break;
                    case 'n':
                        *_tokenStringEnd = '\n';
                        ++_tokenStringEnd;
                        break;
                    case 'r':
                        *_tokenStringEnd = '\r';
                        ++_tokenStringEnd;
                        break;
                    case 't':
                        *_tokenStringEnd = '\t';
                        ++_tokenStringEnd;
                        break;
                    case 'u':
                    {
                        int hex = (char_to_num(_cursor[0]) << 12) | (char_to_num(_cursor[1]) << 8) | (char_to_num(_cursor[2]) << 4) | char_to_num(_cursor[3]);
                        _cursor += 4;
                        if (hex < 0x80) {
                            _tokenStringEnd[0] = hex;
                            ++_tokenStringEnd;
                        } else if (hex < 0x800) {
                            _tokenStringEnd[0] = 0xc0 | (hex >> 6);
                            _tokenStringEnd[1] = 0x80 | (hex & 0x3f);
                            _tokenStringEnd += 2;
                        } else if (hex < 0x10000) {
                            _tokenStringEnd[0] = 0xe0 | (hex >> 12);
                            _tokenStringEnd[1] = 0x80 | ((hex >> 6) & 0x3f);
                            _tokenStringEnd[2] = 0x80 | (hex & 0x3f);
                            _tokenStringEnd += 3;
                        }
                        
                    }
                        
                        break;
                    default:
                        *_tokenStringEnd = d;
                        ++_tokenStringEnd;
                }
            } else {
                if (c == '"') {
                    return;
                } else {
                    *_tokenStringEnd = c;
                    ++_tokenStringEnd;
                }
            }
        }
    }
    
}
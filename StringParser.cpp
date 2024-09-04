//
// Created by Rakeen Mazumder on 8/31/24.
//

#include "StringParser.h"

StringParser::StringParser() {}

bool StringParser::parse(char &c) {
    // single quote
    if ((c == '\'') && (mode == modes::nonstring)) {
        mode = modes::string_single;
        return true;
    }
    if ((c == '\'') && (mode == modes::string_single)) {
        mode = modes::nonstring;
        return false;
    }
    if ((c == '\"') && (mode == modes::nonstring)) {
        mode = modes::string_double;
        return true;
    }
    if ((c == '\"') && (mode == modes::string_double)) {
        mode = modes::nonstring;
        return false;
    }

    if ((mode == modes::string_single) || (mode == modes::string_double)) {
        return true;
    }

    /**/

    return false;
}
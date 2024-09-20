//
// Created by Rakeen Mazumder on 8/31/24.
//

#include "StringParser.h"

StringParser::StringParser() {}

bool StringParser::parse(char &c) {
    switch(c) {
        case '\'':
            // code block
            if (mode == modes::nonstring) {
                mode = modes::string_single;
                return true;
            }
            if (mode == modes::string_single) {
                mode = modes::nonstring;
                return false;
            }
            if (mode == modes::string_double) {
                return true;
            }
            if (mode == modes::escape) {
                mode = prev_mode;
                return true;
            }

        case '\"':
            // code block
            if (mode == modes::nonstring) {
                mode = modes::string_double;
                return true;
            }
            if (mode == modes::string_double) {
                mode = modes::nonstring;
                return false;
            }
            if (mode == modes::string_single) {
                return true;
            }
            if (mode == modes::escape) {
                mode = prev_mode;
                return true;
            }

        case '\\':
            // code block
            prev_mode = mode;
            mode = modes::escape;
            return true;

        default:
            if (mode == modes::nonstring)
                return false;
            if (mode == modes::escape)
                mode = prev_mode;
            return true;
    }
}

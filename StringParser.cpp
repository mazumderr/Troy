//
// Created by Rakeen Mazumder on 8/31/24.
//

#include "StringParser.h"

StringParser::StringParser() {}

/**
 * @brief handles string logic for SourceReader
 *  returns true while in string, else returns false
 *  feeds SourceReader strings, character by character
 *
 *  @pre strings can be single or double quoted
 *
 * @param c character being evaluated from c file
 */
bool StringParser::parse(char &c) {
    switch(c) {
        // single quote string
        case '\'':
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
        // double quote string
        case '\"':
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
        // escape character
        case '\\':
            prev_mode = mode;
            mode = modes::escape;
            return true;

        default:
            if (mode == modes::nonstring)   // characters not in a string
                return false;
            if (mode == modes::escape)      // going back into string mode once escape character finishes
                mode = prev_mode;
    }
    return true;
}

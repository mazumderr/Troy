//
// Created by ARGONAUTS on 8/31/24.
//

#ifndef SOURCEREADER_STRINGPARSER_H
#define SOURCEREADER_STRINGPARSER_H

#include <iostream>
#include <fstream>

class StringParser {
    public:
        StringParser();
        bool parse(char &c);
    private:
        enum class modes {
            string_single, nonstring, string_double, escape
        };
        modes mode = modes::nonstring;
        modes prev_mode;
        char ch;
};


#endif //SOURCEREADER_STRINGPARSER_H

//
// Created by ARGONAUTS on 8/31/24.
//

#ifndef SOURCEREADER_STRINGPARSER_H
#define SOURCEREADER_STRINGPARSER_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class StringParser {
    public:
        StringParser();
        bool parse(char &c);
    private:
        enum class modes {
            string_single, nonstring, string_double
        };
        modes mode = modes::nonstring;
        char ch;
};


#endif //SOURCEREADER_STRINGPARSER_H

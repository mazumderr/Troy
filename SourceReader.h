//
// Created by Rakeen Mazumder on 8/29/24.
//

#ifndef SOURCEREADER_SOURCEREADER_H
#define SOURCEREADER_SOURCEREADER_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "StringParser.h"



class SourceReader {
    public:
        SourceReader(const std::string &filename); //constructor
        // opening file and converting it to input stream
        bool processSource(char &c);

    private:
        std::string sourceFile;
        enum class modes {
            normal, single, block, eocb, string_single, string_double
        };
        modes mode = modes::normal;
        int line_count = 1;
        std::ifstream inputStream;
        StringParser stringParser;
        int begin_comment = 1;

};


#endif //SOURCEREADER_SOURCEREADER_H

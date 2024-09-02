//
// Created by Rakeen Mazumder on 8/29/24.
//

#ifndef SOURCEREADER_SOURCEREADER_H
#define SOURCEREADER_SOURCEREADER_H

#include <sstream>
#include "StringParser.h"


class SourceReader {
    public:
        SourceReader();
        SourceReader(const std::string &filename); //constructor
        // opening file and converting it to input stream
        bool processSource(char &c);
        void open(const std::string &filename);

        void unget(const std::string &str);

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

        std::stringstream ungetBuffer = std::stringstream(std::string());
};


#endif //SOURCEREADER_SOURCEREADER_H

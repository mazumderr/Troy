//
// Created by Rakeen Mazumder, Joshua Lozano on 8/29/24.
//

#include "SourceReader.h"
// constructor
SourceReader::SourceReader() {
}

SourceReader::SourceReader(const std::string &filename) {
    open(filename);
}

void SourceReader::open(const std::string &filename) {
    sourceFile = filename;
    inputStream.open(sourceFile);
}

/**
 * @brief "unget" a string
 *  the next few calls to processSource will just return the string
 * 
 * @pre don't you dare unget a string that contains a \n.
 *      That would throw off the pos and line_count variables really badly.
 *      Only villains do that.
 * @param str string to artificially put into the source file
 */
void SourceReader::unget(const std::string &str) {
    //clumsily grab anything in the ungetBuffer that isn't consumed yet
    std::string curBuff = "";
    char c;
    while (ungetBuffer.get(c)) {
        curBuff += c;
    }
    //stick on the requested string
    curBuff += str;
    //track that we just went back x amount
    pos -= str.length();
    ungetBuffer = std::stringstream(curBuff);
}

bool SourceReader::processSource(char &c) {

    //inputStream.open(sourceFile,std::ios::in);
    // Make sure file exists
    if (!inputStream.is_open()) {
        std::cerr << "File not found!" << std::endl;
        return false;
    }

    ++pos;
    //if there's any ungetBuffer to work with, start there
    if (ungetBuffer.get(c)) {
        return true;
    }
    // Read file character by character
    inputStream.get(c);

    if (inputStream.eof()) {
        if (mode == modes::block) {
            std::cerr << "ERROR: Program contains C-style, unterminated comment on line " << begin_comment;
            inputStream.close();
            return false;
        }
        if (stringParser.parse(c)) {                // unterminated string error
            std::cerr << "ERROR: Program contains unterminated string!";
            inputStream.close();
            return false;
        }
        inputStream.close();
        return false;
    }

    //inputStream.get(c);
    // check for strings
    /**/
    if ((mode == modes::normal) && (stringParser.parse(c))) { // string mode
        return true;
    }


    if (c == '/') {                                // comment found
        if (inputStream.peek() == '/') {          // single comment state
            mode = modes::single;
        }
        else if ((inputStream.peek() == '*') && mode != modes::single) {
            mode = modes::block;                   // comment block state
            begin_comment = line_count;
        }
    }
    if (c == '*') {
        if (inputStream.peek() == '/') {            // terminate comment block, switch modes
            if (mode == modes::normal) {
                std::cerr << "ERROR: Program contains C-style, unterminated comment on line " << line_count;
                inputStream.close();
                return false;
            }
            //c = ' ';
            if (mode == modes::block) {              // mode switch when reaching end of comment block
                mode = modes::eocb;
                c = ' ';
                return true;
            }
        }
        //return true;
    }
    if (c == '\n') {                               // newline
        line_count++;
        pos = 0;
        if (mode == modes::single) {
            mode = modes::normal;
            return true;
        }
        if (mode == modes::block) {
            return true;
        }
    }
    if ((mode == modes::single) || (mode == modes::block)) {          // single and block comment
        c = ' ';
        return true;
    }
    if (mode == modes::eocb) {
        c = ' ';
        mode = modes::normal;
    }
    /*
    if (mode == modes::block) {                     // block comment
        c = ' ';
        return true;
    }
    */

    return true;                                    // source code
}
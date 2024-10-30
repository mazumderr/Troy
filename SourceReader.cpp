//
// Created by Rakeen Mazumder, Joshua Lozano on 8/29/24.
//

#include "SourceReader.h"

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
/**
 * @brief strips comments from source code
 *  returns the source character by character
 *
 * @param c character being evaluated from c file
 */
bool SourceReader::processSource(char &c) {

    // Make sure file exists
    if (!inputStream.is_open()) {
        std::cerr << "File not found!" << std::endl;
        return false;
    }

    ++pos;
    // if there's any ungetBuffer to work with, start there
    if (ungetBuffer.get(c)) {
        return true;
    }

    // Read file character by character
    inputStream.get(c);
    //EOF and EOF errors
    if (inputStream.eof()) {
        if (mode == modes::block) {
            std::cerr << "ERROR: Program contains C-style, unterminated comment on line "
            << begin_comment << std::endl;
        }
        if (stringParser.parse(c)) {
            std::cerr << "ERROR: Program contains unterminated string!" << std::endl;
        }
        inputStream.close();
        return false;
    }
    // string handling
    if ((mode == modes::normal) && (stringParser.parse(c))) {
        return true;
    }
    // DFA
    switch (c) {
        // begin comment
        case '/':
            if (inputStream.peek() == '/') {
                mode = modes::single;
            }
            else if (inputStream.peek() == '*' && mode != modes::single) {
                mode = modes::block;
                begin_comment = line_count;
            }
            break;
        // end comment
        case '*':
            if (inputStream.peek() == '/') {
                if (mode == modes::normal) {
                    std::cerr << "ERROR: Program contains C-style, unterminated comment on line "
                    << line_count << std::endl;
                    inputStream.close();
                    return false;
                }
                if (mode == modes::block) {
                    mode = modes::eocb;
                    c = ' ';
                    return true;
                }
            }
            break;
        // newline
        case '\n':
            line_count++;
            pos = 0;
            if (mode == modes::single) {
                mode = modes::normal;
                return true;
            }
            if (mode == modes::block) {
                return true;
            }
        // none of the above
        default:
            break;
    }
    // modes
    if ((mode == modes::single) || (mode == modes::block)) {
        c = ' ';
    }
    if (mode == modes::eocb) {
        c = ' ';
        mode = modes::normal;
    }
    return true;
}
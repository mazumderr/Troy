#include "Scanner.hpp"
#include "..\StringParser.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

string getReadableTokenType(TokenType t) {
  //regex to build these:
  //^(\s+)(\w+),*
  //$1case TokenType::$2: return "$2";
  switch (t) {
    case TokenType::NONE: return "NONE";
    case TokenType::IDENTIFIER: return "IDENTIFIER";
    case TokenType::LEFT_PARENTHESIS: return "LEFT_PARENTHESIS";
    case TokenType::RIGHT_PARENTHESIS: return "RIGHT_PARENTHESIS";
    case TokenType::SEMICOLON: return "SEMICOLON";
    case TokenType::LEFT_BRACE: return "LEFT_BRACE";
    case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
    case TokenType::OPERATOR: return "OPERATOR";
    case TokenType::ASSIGNMENT: return "ASSIGNMENT";
    case TokenType::INTEGER: return "INTEGER";
    case TokenType::BOOLEAN_EQUAL: return "BOOLEAN_EQUAL";
    case TokenType::SINGLE_QUOTE: return "SINGLE_QUOTE";
    case TokenType::DOUBLE_QUOTE: return "DOUBLE_QUOTE";
    case TokenType::STRING: return "STRING";
    case TokenType::COMMA: return "COMMA";
  }
}

Scanner::Scanner() {
}

Scanner::Scanner(const string &fname) {
  open(fname);
}

void Scanner::open(const string &fname) {
  sr.open(fname);
}

void Scanner::error(string msg) {
  cout << "Scanner: " << msg << " at " << sr.getLine() << ":" << sr.getPos() << endl;
  exit(-1);
}

list<Token> Scanner::getTokens() {
  list<Token> l;

  //Forgive me, but I have to go all out on macros
  //just this once

  //(\s+)(\w)
  //$1case '$2': \\\n$1
  #define case_af \
    case 'a': \
    case 'b': \
    case 'c': \
    case 'd': \
    case 'e': \
    case 'f':
    
  #define case_AF \
    case 'A': \
    case 'B': \
    case 'C': \
    case 'D': \
    case 'E': \
    case 'F': \
    
  #define case_afAF \
    case_af \
    case_AF

  #define case_az \
    case_af \
    case 'g': \
    case 'h': \
    case 'i': \
    case 'j': \
    case 'k': \
    case 'l': \
    case 'm': \
    case 'n': \
    case 'o': \
    case 'p': \
    case 'q': \
    case 'r': \
    case 's': \
    case 't': \
    case 'u': \
    case 'v': \
    case 'w': \
    case 'x': \
    case 'y': \
    case 'z':
    
  #define case_AZ \
    case_AF \
    case 'G': \
    case 'H': \
    case 'I': \
    case 'J': \
    case 'K': \
    case 'L': \
    case 'M': \
    case 'N': \
    case 'O': \
    case 'P': \
    case 'Q': \
    case 'R': \
    case 'S': \
    case 'T': \
    case 'U': \
    case 'V': \
    case 'W': \
    case 'X': \
    case 'Y': \
    case 'Z': 
    
  #define case_azAZ \
    case_az \
    case_AZ

  #define case_azAZ_ \
    case_azAZ \
    case '_':

  #define case_09 \
    case '0': \
    case '1': \
    case '2': \
    case '3': \
    case '4': \
    case '5': \
    case '6': \
    case '7': \
    case '8': \
    case '9': 
  
  #define case_azAZ09_ \
    case_09 \
    case_azAZ_

  enum class ss {
    IDLE,
    INT,
    PLUSMINUS,
    INTEGER,
    EQUAL,
    STRING,
  };

  ss state = ss::IDLE;

  string spelling = "";

  StringParser sp;

  char c;
  while (sr.processSource(c)) {
    switch (state) {
      case ss::IDLE: {
        switch (c) {
          //identifier
          case_azAZ_ {
            spelling += c;

            bool stop = false;

            while (!stop && sr.processSource(c)) {
              switch (c) {
                case_azAZ09_ {
                  spelling += c;
                } break;

                default: {
                  stop = true;
                }
              }
            }
            
            sr.unget(string{c});
            
            l.emplace_back(Token (
              spelling,
              TokenType::IDENTIFIER,
              sr.getLine(),
              sr.getPos()
            ));

            spelling = "";

          } break;

          case_09 {
            spelling += c;
            state = ss::INTEGER;
          } break;

          case '(' : {
            l.emplace_back(Token (
              string{c},
              TokenType::LEFT_PARENTHESIS,
              sr.getLine(),
              sr.getPos()
            ));
          } break;

          case ')' : {
            l.emplace_back(Token (
              string{c},
              TokenType::RIGHT_PARENTHESIS,
              sr.getLine(),
              sr.getPos()
            ));
          } break;

          case '{' : {
            l.emplace_back(Token (
              string{c},
              TokenType::LEFT_BRACE,
              sr.getLine(),
              sr.getPos()
            ));
          } break;

          case '}' : {
            l.emplace_back(Token (
              string{c},
              TokenType::RIGHT_BRACE,
              sr.getLine(),
              sr.getPos()
            ));
          } break;

          case ';' : {
            l.emplace_back(Token (
              string{c},
              TokenType::SEMICOLON,
              sr.getLine(),
              sr.getPos()
            ));
          } break;

          case ',' : {
            l.emplace_back(Token (
              string{c},
              TokenType::COMMA,
              sr.getLine(),
              sr.getPos()
            ));
          } break;

          case '+' :
          case '-' : {
            spelling += c;
            state = ss::PLUSMINUS;
          } break;

          case '=' : {
            spelling += c;
            state = ss::EQUAL;
          } break;

          case '\'': {
            l.emplace_back(Token (
              string{c},
              TokenType::SINGLE_QUOTE,
              sr.getLine(),
              sr.getPos()
            ));
            state = ss::STRING;
            sp.parse(c);
          } break;

          case '\"': {
            l.emplace_back(Token (
              string{c},
              TokenType::DOUBLE_QUOTE,
              sr.getLine(),
              sr.getPos()
            ));
            state = ss::STRING;
            sp.parse(c);
          } break;

          default: {
            //FIXME: should the idle state report errors on unexpected characters?
          }
        }
      } break;

      case ss::PLUSMINUS: {
        switch (c) {

          case_09 {
            spelling += c;
            state = ss::INTEGER;
          } break;

          default: {
            sr.unget(string{c});
            l.emplace_back(Token(
              spelling,
              TokenType::OPERATOR,
              sr.getLine(),
              sr.getPos()
            ));
            spelling = "";
            state = ss::IDLE;
          }
        }
      } break;

      case ss::INTEGER: {
        switch (c) {
          case_09 {
            spelling += c;

            bool stop = false;

            while (!stop && sr.processSource(c)) {
              switch (c) {
                case_09 {
                  spelling += c;
                } break;

                default: {
                stop = true;
                }
              }
            }

            sr.unget(string{c});
            
            l.emplace_back(Token (
              spelling,
              TokenType::INTEGER,
              sr.getLine(),
              sr.getPos()
            ));

            spelling = "";
          } break;

          default: {
            sr.unget(string{c});
            
            l.emplace_back(Token (
              spelling,
              TokenType::INTEGER,
              sr.getLine(),
              sr.getPos()
            ));

            spelling = "";
            state = ss::IDLE;
          }
        }
      } break;

      case ss::EQUAL : {
        switch (c) {
          case '=': {
            spelling += "=";

            l.emplace_back( Token(
              spelling,
              TokenType::BOOLEAN_EQUAL,
              sr.getLine(),
              sr.getPos()
            ));

            spelling = "";
            state = ss::IDLE;
          } break;

          default: {
            sr.unget(string{c});

            l.emplace_back( Token(
              spelling,
              TokenType::ASSIGNMENT,
              sr.getLine(),
              sr.getPos()
            ));

            spelling = "";
            state = ss::IDLE;
          }
        }
      } break;

      case ss::STRING: {

        while (sp.parse(c)) {
          spelling += c;
          if (!sr.processSource(c)) break;
        }

        l.emplace_back( Token(
          spelling,
          TokenType::STRING,
          sr.getLine(),
          sr.getPos()
        ));

        state = ss::IDLE;
        spelling = "";

        if (c == '\'')
          l.emplace_back( Token(
            string{c},
            TokenType::SINGLE_QUOTE,
            sr.getLine(),
            sr.getPos()
          ));
        else if (c == '\"')
          l.emplace_back( Token(
            string{c},
            TokenType::DOUBLE_QUOTE,
            sr.getLine(),
            sr.getPos()
          ));
        else {
          error("Unexpected character has terminated string");
        }

      } break;

      default: {
        error("Entered unhandled state");
      }
    }
  }

  return l;
}
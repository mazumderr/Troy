#include "Scanner.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

Scanner::Scanner() {
}

Scanner::Scanner(const string &fname) {
  open(fname);
}

void Scanner::open(const string &fname) {
  sr.open(fname);
}

void Scanner::error(string msg) {
  cout << "Scanner: " << msg << " at " << line << ":" << pos << endl;
  exit(-1);
}

list<Token> Scanner::getTokens() {
  list<Token> l;

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
  };

  ss state = ss::IDLE;

  string spelling = "";

  char c;
  while (sr.processSource(c)) {
    switch (state) {
      case ss::IDLE: {
        switch (c) {
          //valid identifier
          case_azAZ_ {
            spelling += c;

            bool stop = false;

            while (!stop && sr.processSource(c)) {
              switch (c) {
                case_azAZ09_ {
                  spelling += c;
                } break;

                default: {
                  sr.unget(string{c});
                  cout << spelling << endl;
                  spelling = "";
                  stop = true;
                }
              }
            }
          } break;

          case '(' : {
            cout << c << endl;
          } break;

          case ')' : {
            cout << c << endl;
          } break;

          default: {
            //FIXME: should the idle state report errors on unexpected characters?
          }
        }
      } break;

      default: {
        error("Entered unhandled state");
      }
    }
  }

  return l;
}
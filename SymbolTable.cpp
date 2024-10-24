#include "SymbolTable.hpp"

#include <iostream>

string getReadableSymbolType(const SymbolType& t) {
  //regex to generate these:
  //(\s+)(\w+),
  //$1$1case SymbolType::$2: return "$2";
  switch(t) {
    case SymbolType::NONE: return "NONE";
    case SymbolType::FUNCTION: return "FUNCTION";
    case SymbolType::DATATYPE: return "DATATYPE";
    case SymbolType::INT: return "INT";
    case SymbolType::CHAR: return "CHAR";
    case SymbolType::BOOL: return "BOOL";
    default:
      //this should never, ever happen
      cerr << "getPrintableSymbolType got a bad SymbolType" << endl;
      exit(-1);
  }


}
#include "SymbolTable.hpp"

#include <iostream>

string getReadableSymbolType(const SymbolType& t) {
  //regex to generate these:
  //(\s+)(\w+),
  //$1$1case SymbolType::$2: return "$2";
  switch(t) {
    case SymbolType::NONE: return "NONE";
    case SymbolType::FUNCTION: return "FUNCTION";
    case SymbolType::PROCEDURE: return "PROCEDURE";
    case SymbolType::INT: return "INT";
    case SymbolType::CHAR: return "CHAR";
    case SymbolType::BOOL: return "BOOL";
    default:
      //this should never, ever happen
      cerr << "getPrintableSymbolType got a bad SymbolType" << endl;
      exit(-1);
  }
}

void Symbol::print(bool printITypeLine) {
  //I am fully aware that this would have been a lot simpler if my symbol structure
  //more closely reflected the professor's output
  //but I refuse to compromise on that
  cout << "      IDENTIFIER_NAME: " << name << endl;
  
  if (printITypeLine) {
    cout << "      IDENTIFIER_TYPE: " << (
      type == SymbolType::FUNCTION || type == SymbolType::PROCEDURE ?
        getReadableSymbolType(type)
        :
        "datatype"
    ) << endl;
  }
  
  cout << "             DATATYPE: " << (
    type == SymbolType::PROCEDURE ?
      "NOT APPLICABLE"
      :
      type == SymbolType::FUNCTION ?
        getReadableSymbolType(((CallableSymbol*)this)->returnType)
        :
        getReadableSymbolType(type)
  ) << endl;
  cout << "    DATATYPE_IS_ARRAY: " << (
    isArray ?
      "yes"
      :
      "no"
  ) << endl;
  cout << "  DATATYPE_ARRAY_SIZE: " << arraySize << endl;
  cout << "                SCOPE: " << scope << endl;
  cout << endl;
}
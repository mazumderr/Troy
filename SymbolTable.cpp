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

// void Symbol::print(bool printITypeLine) {
//   //I am fully aware that this would have been a lot simpler if my symbol structure
//   //more closely reflected the professor's output
//   //but I refuse to compromise on that
//   cout << "      IDENTIFIER_NAME: " << name << endl;
  
//   if (printITypeLine) {
//     cout << "      IDENTIFIER_TYPE: " << (
//       type == SymbolType::FUNCTION || type == SymbolType::PROCEDURE ?
//         getReadableSymbolType(type)
//         :
//         "datatype"
//     ) << endl;
//   }
  
//   cout << "             DATATYPE: " << (
//     type == SymbolType::PROCEDURE ?
//       "NOT APPLICABLE"
//       :
//       type == SymbolType::FUNCTION ?
//         getReadableSymbolType(this->returnType)
//         :
//         getReadableSymbolType(type)
//   ) << endl;
//   cout << "    DATATYPE_IS_ARRAY: " << (
//     isArray ?
//       "yes"
//       :
//       "no"
//   ) << endl;
//   cout << "  DATATYPE_ARRAY_SIZE: " << arraySize << endl;
//   cout << "                SCOPE: " << scope << endl;
//   cout << endl;
// }

SymbolTable* SymbolTable::creatSubScope() {
  subScopes.push_back(SymbolTable());
  subScopes.back().parentTable = this;
  return &(subScopes.back());
}

SymbolTable* SymbolTable::getParentTable() {
  return parentTable;
}

string SymbolTable::addSymbolAndDescend(Symbol* s, SymbolTable* &t) {
  string str = addSymbol(s);
  if (str != "") {
    return str;
  }
  t = creatSubScope();
  symbols.back()->childTable = t;
  return "";
}

string SymbolTable::addSymbol(Symbol* s) {  
  //make sure the symbol's name is allowed
  string forbiddenNames[] = {
    "char",
    "function",
    "procedure",
    "void",
    "printf",
    "int",
  };
  #define forbiddenCount 6

  for (unsigned int i = 0; i < forbiddenCount; ++i) {
    if (s->name == forbiddenNames[i])
      return "reserved word " +
        s->name +
        " cannot be used for the name of a " +
        (s->type == SymbolType::FUNCTION ? "function" : "variable")
        ;
  }
  
  SymbolTable* curTable = this;

  // cout << "trying to add symbol " << s->name << " to table " << this << endl;

  do {
    for (auto existingSymbol: curTable->symbols) {
      // cout << "\tchecking existing symbol " << existingSymbol->name << endl;
      if (existingSymbol->name == s->name) {
        return "variable \"" + s->name + "\" is already defined " + 
        (curTable == this ? "locally" : "globally");
      }
    }
    curTable = curTable->parentTable;
  }
  while (curTable != nullptr);

  symbols.push_back(s);
  
  return "";
}

void SymbolTable::print(){
  
}
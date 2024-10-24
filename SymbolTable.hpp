#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <string>
#include <list>

using namespace std;

enum class SymbolType {
  NONE,
  FUNCTION,
  DATATYPE,
  INT,
  CHAR,
  BOOL,
};

struct Symbol {
  string name = "";
  SymbolType type = SymbolType::NONE;
};

struct CallableSymbol: public Symbol {
  SymbolType returnType = SymbolType::NONE;
  list<Symbol*> arguments;
};

string getReadableSymbolType(const SymbolType&);

#endif
#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <string>
#include <list>

using namespace std;

enum class SymbolType {
  NONE,
  FUNCTION,
  PROCEDURE,
  INT,
  CHAR,
  BOOL,
};

struct Symbol {
  string name = "";
  SymbolType type = SymbolType::NONE;
  unsigned int scope = 0;
  bool isArray = false;
  unsigned int arraySize = 0;

  void print();
};

struct CallableSymbol: public Symbol {
  SymbolType returnType = SymbolType::NONE;
  list<Symbol*>* arguments = nullptr;
};

string getReadableSymbolType(const SymbolType&);

#endif
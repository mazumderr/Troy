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

class SymbolTable;

struct Symbol {
  string name = "";
  SymbolType type = SymbolType::NONE;
  bool isArray = false;
  unsigned int arraySize = 0;

  SymbolType returnType = SymbolType::NONE;
  SymbolTable* childTable;
};

string getReadableSymbolType(const SymbolType&);

class SymbolTable {
  public:
    void print();
    SymbolTable* creatSubScope();
    SymbolTable* getParentTable();
    [[nodiscard]] string addSymbol(Symbol*);
    [[nodiscard]] string addSymbolAndDescend(Symbol*, SymbolTable*&);
    
  private:
    list<Symbol*> symbols;
    list<SymbolTable> subScopes;
    SymbolTable* parentTable = nullptr;

    void print(const unsigned int curScope);
    void print(const Symbol &s, const unsigned int curScope);
};

#endif
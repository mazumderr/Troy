#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <string>
#include <list>
#include "CodeNode.hpp"

using namespace std;

enum class SymbolType {
  NONE,
  FUNCTION,
  PROCEDURE,
  INT,
  CHAR,
  BOOL,
};

class CodeScope;
class CodeNode;

struct Symbol {
  string name = "";
  SymbolType type = SymbolType::NONE;
  bool isArray = false;
  unsigned int arraySize = 0;

  SymbolType returnType = SymbolType::NONE;
  CodeScope* myScope = nullptr;
  CodeNode* myDeclaration = nullptr;

  void assign(int);

  int* value = nullptr;
};

string getReadableSymbolType(const SymbolType&);

class CodeScope {
  public:
    ~CodeScope();
    void print();
    CodeScope* createSubScope();
    CodeScope* getParentScope();
    list<Symbol*> getParameters();
    Symbol* lookupSymbol(const string&);
    [[nodiscard]] string addSymbol(Symbol*);
    [[nodiscard]] string addParameter(Symbol*);
    [[nodiscard]] string addSymbolAndDescend(Symbol*, CodeScope*&);
    
  private:
    list<Symbol*> symbols;
    list<Symbol*> parameters;
    list<CodeScope> subScopes;
    CodeScope* parentScope = nullptr;

    void print(const unsigned int curScope);
    void print(const Symbol &s, const unsigned int curScope, bool);
    bool checkForbidden(const string& s);
};

#endif
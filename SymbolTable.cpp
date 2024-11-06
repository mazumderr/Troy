#include "SymbolTable.hpp"
#include "Tools.hpp"

#include <iostream>

/**
 * @brief Get a friendly name for symbol types
 * 
 * @param t symboltype to translate to a string
 * @return string 
 */
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

/**
 * @brief Destroy the Symbol Table:: Symbol Table object
 * 
 */
CodeScope::~CodeScope(){
  for (auto s: symbols) {
    delete s;
  }

  for (auto p: parameters) {
    delete p;
  }
}

/**
 * @brief Create a subscope for this scope (function bodies, if statements, etc)
 * 
 * @return CodeScope* pointer to the scope we just created
 */
CodeScope* CodeScope::createSubScope() {
  subScopes.push_back(CodeScope());
  subScopes.back().parentScope = this;
  return &(subScopes.back());
}

/**
 * @brief get the scope that this scope belongs to; nullptr if this is the global scope
 * 
 * @return CodeScope* ptr
 */
CodeScope* CodeScope::getParentScope() {
  return parentScope;
}

/**
 * @brief adds a symbol to the current scope if possible, returns a subscope for that symbol
 * 
 * @param s symbol to add
 * @param t output CodeScope pointer
 * @return string error (empty if success, or full of descriptive text)
 */
string CodeScope::addSymbolAndDescend(Symbol* s, CodeScope* &t) {
  string str = addSymbol(s);
  if (str != "") {
    return str;
  }
  t = createSubScope();
  symbols.back()->myScope = t;
  return "";
}

/**
 * @brief check if a string is a forbidden keyword
 * 
 * @param s word to check
 * @return true if word is reserved
 */
bool CodeScope::checkForbidden(const string& s) {
  // cout << "checkForbidden " << s << endl;
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
    if (s == forbiddenNames[i])
      return true;
  }

  return false;
}

/**
 * @brief adds a parameter to the current scope
 * 
 * @param s symbol to add
 * @return string error (empty if successful)
 */
string CodeScope::addParameter(Symbol* s) {
  //make sure the symbol's name is allowed
  if (checkForbidden(s->name))
    return "reserved word " +
      s->name +
      " cannot be used for the name of a variable";
  
  for (auto existingParameter: parameters) {
    if (existingParameter->name == s->name) {
        return "variable \"" + s->name + "\" is already defined locally";
    }
  }

  parameters.push_back(s);

  return "";
}

/**
 * @brief adds a symbol to the scope
 * 
 * @param s symbol to add
 * @return string error (empty if successful)
 */
string CodeScope::addSymbol(Symbol* s) {
  //make sure the symbol's name is allowed
  if (checkForbidden(s->name))
    return "reserved word " +
      s->name +
      " cannot be used for the name of a " +
      (s->type == SymbolType::FUNCTION ? "function" : "variable");
  
  CodeScope* curTable = this;

  // cout << "trying to add symbol " << s->name << " to table " << this << endl;

  do {
    for (auto existingSymbol: curTable->symbols) {
      // cout << "\tchecking existing symbol " << existingSymbol->name << endl;
      if (existingSymbol->name == s->name) {
        return "variable \"" + s->name + "\" is already defined " + 
        (curTable == this ? "locally" : "globally");
      }
    }
    //FIXME: I don't think this actually needs to happen outside of the symbol table tests
    for (auto existingSymbol: curTable->parameters) {
      // cout << "\tchecking existing symbol " << existingSymbol->name << endl;
      if (existingSymbol->name == s->name) {
        return "variable \"" + s->name + "\" is already defined " + 
        (curTable == this ? "locally" : "globally");
      }
    }
    curTable = curTable->parentScope;
  }
  while (curTable != nullptr);

  symbols.push_back(s);
  
  return "";
}






////////////////////////////////////////////////////////////////////
//                HERE BE DRAGONS
//                  code below this code is ugly and bad
//                  and exists only to pass Prof Bruce's tests
////////////////////////////////////////////////////////////////////


void CodeScope::print() {
  int curScope = 0;
  int highestScope = 0;
  for (auto s: symbols) {
    if (s->type == SymbolType::PROCEDURE || s->type == SymbolType::FUNCTION) {
      ++highestScope;
      curScope = highestScope;
      print(*s, curScope, true);
    }
    else print(*s, 0, true);

    if (s->myScope) s->myScope->print(curScope);
  }

  curScope = 0;
  highestScope = 0;
  for (auto s: symbols) {
    if (s->type == SymbolType::PROCEDURE || s->type == SymbolType::FUNCTION) {
      ++highestScope;
      curScope = highestScope;
    }
    if (s->type == SymbolType::FUNCTION) {
      cout << endl << "   PARAMETER LIST FOR: " << s->name << endl;

      for (auto p: s->myScope->parameters) {
        print(*p, curScope, false);
      }
    }
  }
}

void CodeScope::print(const unsigned int curScope) {
  for (auto s: symbols) {

    print(*s, curScope, true);

    if (s->myScope) s->myScope->print(curScope);
  }
}

void CodeScope::print(const Symbol &s, const unsigned int curScope, bool itline) {
  cout << "      IDENTIFIER_NAME: " << s.name << endl;

  if (itline)
    cout << "      IDENTIFIER_TYPE: " << 
    ((s.type == SymbolType::FUNCTION || s.type == SymbolType::PROCEDURE) ? getReadableSymbolType(s.type) : "datatype")
    << endl;

  cout << "             DATATYPE: " << 
    (s.type == SymbolType::FUNCTION ? 
      getLowercase(getReadableSymbolType(s.returnType))
    :
      (s.type == SymbolType::PROCEDURE ? "NOT APPLICABLE" : getLowercase(getReadableSymbolType(s.type))))
  << endl
  << "    DATATYPE_IS_ARRAY: " << (s.isArray ? "yes" : "no") << endl
  << "  DATATYPE_ARRAY_SIZE: " << s.arraySize << endl
  << "                SCOPE: " << curScope << endl << endl;
}
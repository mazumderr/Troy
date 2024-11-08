/**
 * @file DescentParser.hpp
 * @author Bo Thompson (extralife.xyz)
 * @brief module to create a kind of bad parse tree and detect a few syntax errors
 * @version 0.1
 * @date 2024-10-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef DESCENTPARSER_HPP
#define DESCENTPARSER_HPP

#include "CodeNode.hpp"
#include "Scanner.hpp"
#include "SymbolTable.hpp"
#include "Tools.hpp"
#include <map>

class DescentParser {
  public:
    bool parse(const string&, SyntaxTree*&, CodeScope*&);

  private:
    map<string, SymbolType> typemap = {
      {"int", SymbolType::INT},
      {"char", SymbolType::CHAR},
      {"bool", SymbolType::BOOL},
    };
    string getParameters(list<Token>::iterator&, CodeScope *&);
    void handleArrayDeclaration(list<Token>::iterator& t, Symbol* s);
};

#endif
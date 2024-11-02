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

#include "Scanner.hpp"
#include "SymbolTable.hpp"
#include "Tools.hpp"
#include <map>


  // types of abstract nodes
  // NONE,
  // DECLARATION,
  // BEGIN_BLOCK,
  // END_BLOCK,
  // ASSIGNMENT,
  // IF,
  // RETURN,
  // PRINTF,

class CodeNode {
  public:
    CodeNode(const Token& t);
    ~CodeNode();

    Token* getToken();
    CodeNode* getChild();
    CodeNode* getSibling();
    void setChild(CodeNode*);
    void setSibling(CodeNode*);
    void setToken(Token*);
    void setToken(const Token&);

  private:
    CodeNode* child = nullptr;
    CodeNode* sibling = nullptr;
    Token* token = nullptr;
};

class DescentParser {
  public:
    bool parse(const string&, CodeNode*&, SymbolTable*&);

  private:
    map<string, SymbolType> typemap = {
      {"int", SymbolType::INT},
      {"char", SymbolType::CHAR},
      {"bool", SymbolType::BOOL},
    };
};

#endif
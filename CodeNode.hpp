#ifndef CODENODE_HPP
#define CODENODE_HPP

#include "SymbolTable.hpp"
#include "Scanner.hpp"
#include <vector>

using namespace std;

enum class NodeType {
  NONE,
  ASSIGNMENT,
  DECLARATION,
  EXPRESSION,
  BEGIN,
  END,
  IF,
  ELSE,
  RETURN,
  PRINTF,
  CALL,
  WHILE,
  FOR_INITIAL,
  FOR_CONDITION,
  FOR_INCREMENT,
};

string getPrintableNodeType(NodeType);

class CodeScope;

class CodeNode{
  public:
    ~CodeNode();
    void setType(NodeType);
    NodeType getType();
    void setScope(CodeScope*);
    void setSibling(CodeNode*);
    void setChild(CodeNode*);
    CodeNode* getSibling();
    CodeNode* getChild();
    void setExpression(vector<Token>);
    vector<Token> getExpression();
    
  private:
    CodeNode* sibling = nullptr;
    CodeNode* child = nullptr;
    NodeType type = NodeType::NONE;
    CodeScope* myScope = nullptr;
    vector<Token> expression;
};

class SyntaxTree{
  public:
    ~SyntaxTree();
    void append(CodeNode*);
    void descend(CodeNode*);
    void print();
  private:
    CodeNode* head = nullptr;
    CodeNode* tail = nullptr;
};

#endif
#include "CodeNode.hpp"
#include <iostream>

string getPrintableNodeType(NodeType t) {
  
  //regex to generate these:
  //(\s+)(\w+),
  //$1$1case NodeType::$2: return "$2";
  switch(t) {
    case NodeType::NONE: return "NONE";
    case NodeType::ASSIGNMENT: return "ASSIGNMENT";
    case NodeType::DECLARATION: return "DECLARATION";
    case NodeType::EXPRESSION: return "EXPRESSION";
    case NodeType::BEGIN: return "BEGIN";
    case NodeType::END: return "END";
    case NodeType::IF: return "IF";
    case NodeType::ELSE: return "ELSE";
    case NodeType::RETURN: return "RETURN";
    case NodeType::PRINTF: return "PRINTF";
    case NodeType::CALL: return "CALL";
    case NodeType::WHILE: return "WHILE";
    case NodeType::FOR_INITIAL: return "FOR_INITIAL";
    case NodeType::FOR_CONDITION: return "FOR_CONDITION";
    case NodeType::FOR_INCREMENT: return "FOR_INCREMENT";
    default:
      //this should never, ever happen
      cerr << "getPrintableNodeType got a bad NodeType" << endl;
      exit(-1);
  }
}

CodeNode::~CodeNode() {
  delete sibling;
  delete child;
}

void CodeNode::setSibling(CodeNode *c) {
  if (sibling != nullptr) {
    cerr << "CodeNode: setSiblingX2" << endl;
    exit(-1);
  }
  sibling = c;
}

void CodeNode::setChild(CodeNode *c) {
  if (child != nullptr) {
    cerr << "CodeNode: setChildX2" << endl;
    exit(-1);
  }
  child = c;
}

CodeNode* CodeNode::getSibling() {
  return sibling;
}

CodeNode* CodeNode::getChild() {
  return child;
}

void CodeNode::setExpression(vector<Token> e) {
  expression = e;
}

vector<Token> CodeNode::getExpression() {
  return expression;
}

void CodeNode::setType(NodeType t) {
  if (type != NodeType::NONE) {
    cerr << "CodeNode: setTypeX2" << endl;
    exit(-1);
  }
  type = t;
}

NodeType CodeNode::getType() {
  return type;
}

void CodeNode::setScope(CodeScope *s) {
  if (myScope != nullptr) {
    cerr << "CodeNode: setScopeX2" << endl;
    exit(-1);
  }
  myScope = s;
}

SyntaxTree::~SyntaxTree() {
  delete head;
  delete tail;
}

void SyntaxTree::append(CodeNode *c) {
  if (head == nullptr) {
    head = c;
    tail = c;
  }
  else {
    tail->setSibling(c);
    tail = c;
  }
}

void SyntaxTree::descend(CodeNode *c) {
  if (head == nullptr) {
    head = c;
    tail = c;
  }
  else {
    tail->setChild(c);
    tail = c;
  }
}

void SyntaxTree::print() {
  CodeNode* c = head;
  while (c != nullptr) {
    cout << getPrintableNodeType(c->getType());
    // for (auto t: c->getExpression()) {
    //   cout << " " << t.getSpelling() << ",";
    // }
    if (c->getSibling() == nullptr) {
      cout << endl;
      c = c->getChild();
    }
    else {
      cout << "\t";
      c = c->getSibling();
    }
  }
}
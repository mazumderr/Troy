#include "DescentParser.hpp"

CodeTree::CodeTree(const Token &t) {
  token = new Token(t);
}

Token* CodeTree::getToken() {
  return token;
}

CodeTree *CodeTree::getChild() {
  return child;
}

CodeTree *CodeTree::getSibling() {
  return sibling;
}

void CodeTree::setChild(CodeTree *c) {
  child = c;
}

void CodeTree::setSibling(CodeTree *s) {
  sibling = s;
}



DescentParser::DescentParser() {
}

DescentParser::DescentParser(const string &fname) {
  open(fname);
}

void DescentParser::open(const string &fname) {
  s.open(fname);
}

CodeTree* DescentParser::parse() {
  list<Token> tokenList = s.getTokens();

  if (tokenList.empty())  return nullptr;

  CodeTree* root = nullptr;
  CodeTree* prevPtr = nullptr;
  bool descend = false;

  enum class ss {
    START,
    IGNORE,
  };

  ss state = ss::START;

  for (auto t = tokenList.begin(); t != tokenList.end(); ++t) {
    auto nt = next(t,1);

    // switch(state) {
    //   case ss::START:
    //     if 
    //   break;

    //   case ss::IGNORE:
    //   break;
    // }

    if (prevPtr != nullptr) {
      if (descend) {
        prevPtr->setChild(new CodeTree(*t));
        prevPtr = prevPtr->getChild();
      }
      else {
        prevPtr->setSibling(new CodeTree(*t));
        prevPtr = prevPtr->getSibling();
      }
    }
    else {
      prevPtr = new CodeTree(*t);
      root = prevPtr;
    }


    if ((t->getType() == TokenType::SEMICOLON) ||
        (t->getType() == TokenType::LEFT_BRACE) ||
        (t->getType() == TokenType::RIGHT_BRACE) ||
          ((nt != tokenList.end()) &&
            (
              (nt->getType() == TokenType::LEFT_BRACE) ||
              (nt->getType() == TokenType::RIGHT_BRACE)
            )
          )
    ) { //child
      descend = true;
    }
    else {
      descend = false;
    }
  }

  return root;
}
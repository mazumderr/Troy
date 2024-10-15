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
    SEEN_IDENTIFIER,
    FUNC_DEF,
    PROC_DEF,
    IGNORE,
  };

  ss state = ss::START;

  for (auto t = tokenList.begin(); t != tokenList.end(); ++t) {
    auto nt = next(t,1);

    //form this node in the tree
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

    //figure out next branch
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

    const string lspell = getLowercase(t->getSpelling());

    //check for errors
    switch(state) {
      case ss::START:
        // cout << t->getLine() << " start of statement: " << t->getSpelling() << endl;
        switch (t->getType()) {
          case TokenType::IDENTIFIER:
            if (lspell == "function") {
              state = ss::IGNORE;
            }
            else if (lspell == "procedure") {
              state = ss::IGNORE;
            }
            else if (lspell == "return") {
              state = ss::IGNORE;
            }
            else {
              state = ss::SEEN_IDENTIFIER;
            }
          break;
          default:
            //if there's more to this statement, we really don't care
            state = ss::IGNORE;
        }
      break;

      case ss::SEEN_IDENTIFIER:
        switch (t->getType()) {
          case TokenType::IDENTIFIER:
            if (wordIsForbidden(lspell)) {
              cout << "Syntax error on line " << t->getLine()
                << ": reserved word \"" << t->getSpelling() << "\" cannot be used for the name of a variable."
                << endl;
              exit(-1);
            }
            state = ss::IGNORE;
          break;
          default:
            state = ss::IGNORE;
        }
      break;

      case ss::IGNORE:
      break;
    }

    //reset for the start of each statement
    if (descend)
      state = ss::START;
  }

  return root;
}

string DescentParser::getLowercase(const string &s) {
  string out;
  
  for (auto c: s) {
    out += tolower(c);
  }

  return out;
}

bool DescentParser::wordIsForbidden(const string &s) {
  string forbidden[] = {
    "char",
    "function",
    "procedure",
    "void",
    "printf",
  };
  #define forbiddenCount 5
  
  for (unsigned int i = 0; i < forbiddenCount; ++i) {
    if (s == forbidden[i]) {
      return true;
    }
  }
  return false;
}

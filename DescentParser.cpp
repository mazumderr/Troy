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
    DECLARED_VARIABLE,
    ARRAY_DECLARATION,
    FUNC_RETURN_TYPE,
    FUNC_NAME,
    FUNC_OPEN,
    FUNC_ARGTYPE,
    FUNC_ARGNAME,
    FUNC_COMMACLOSE,
    PROC_DEF,
    IGNORE,
  };

  enum class fs {
    NONE,
    EXPECTING_FIRST_SEMI,
    EXPECTING_SECOND_SEMI,
  };

  ss state = ss::START;
  fs forState = fs::NONE;

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
              state = ss::FUNC_RETURN_TYPE;
            }
            else if (lspell == "procedure") {
              state = ss::IGNORE;
            }
            else if (lspell == "return") {
              state = ss::IGNORE;
            }
            else if (lspell == "for") {
              forState = fs::EXPECTING_FIRST_SEMI;
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
            state = ss::DECLARED_VARIABLE;
          break;
          default:
            state = ss::IGNORE;
        }
      break;

      case ss::DECLARED_VARIABLE:
        switch (t->getType()) {
          case TokenType::LEFT_BRACKET:
            state = ss::ARRAY_DECLARATION;
          break;
          default:
            state = ss::IGNORE;
        }
      break;

      case ss::ARRAY_DECLARATION:
        switch (t->getType()) {
          case TokenType::INTEGER:
            if (stoi(lspell) <= 0) {
              cout << "Syntax error on line " << t->getLine() << ": array declaration size must be a positive integer." << endl;
              exit(-1);
            }
            state = ss::IGNORE;
          break;
          default:
            state = ss::IGNORE;
        }
      break;

      case ss::FUNC_RETURN_TYPE:
        //don't actually care about what happens here... yet
        state = ss::FUNC_NAME;        
      break;

      case ss::FUNC_NAME:
        switch(t->getType()) {
          case TokenType::IDENTIFIER:
            if (wordIsForbidden(lspell)) {
              cout << "Syntax error on line " << t->getLine()
                << ": reserved word \"" << t->getSpelling() << "\" cannot be used for the name of a function."
                << endl;
              exit(-1);
            }
            state = ss::FUNC_OPEN;
          break;
          default:
            state = ss::IGNORE;
        }
      break;

      case ss::FUNC_OPEN:
        state = ss::FUNC_ARGTYPE;
      break;

      case ss::FUNC_ARGTYPE:
        state = ss::FUNC_ARGNAME;
      break;

      case ss::FUNC_ARGNAME:
        if (wordIsForbidden(lspell)) {
          cout << "Syntax error on line " << t->getLine()
            << ": reserved word \"" << t->getSpelling() << "\" cannot be used for the name of a variable."
            << endl;
          exit(-1);
        }
        state = ss::FUNC_COMMACLOSE;
      break;

      case ss::FUNC_COMMACLOSE: {
        switch (t->getType()) {
          case TokenType::COMMA:
            state = ss::FUNC_ARGTYPE;
          break;
          case TokenType::RIGHT_PARENTHESIS:
            state = ss::IGNORE;
          break;
          default: {}
        }
      }

      case ss::IGNORE:
      default:{}
    }

    //reset for the start of each statement
    if (descend) state = ss::START;
    
    //special handling for for statements
    switch(forState) {
      case fs::EXPECTING_FIRST_SEMI: {
        descend = false;

        if (t->getType() == TokenType::SEMICOLON)
          forState = fs::EXPECTING_SECOND_SEMI;
      };
      break;

      case fs::EXPECTING_SECOND_SEMI:
        descend = false;

        if (t->getType() == TokenType::SEMICOLON)
          forState = fs::NONE;
      break;
      
      default: {};
    }
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
    "int",
  };
  #define forbiddenCount 6
  
  for (unsigned int i = 0; i < forbiddenCount; ++i) {
    if (s == forbidden[i]) {
      return true;
    }
  }
  return false;
}

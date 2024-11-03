/**
 * @file DescentParser.cpp
 * @author Bo Thompson (extralife.xyz)
 * @brief implementation of the CodeNode and DecentParser functions
 * @version 0.1
 * @date 2024-10-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "DescentParser.hpp"

/**
 * @brief Construct a new Code Node:: Code Node object
 * 
 * @param t token to copy and save as ours
 */
CodeNode::CodeNode(const Token &t) {
  setToken(new Token(t));
}

/**
 * @brief Destroy the Code Node:: Code Node object
 * 
 */
CodeNode::~CodeNode() {
  delete child;
  delete sibling;
  delete token;
}

/**
 * @brief gets the token for this node
 * 
 * @return Token* 
 */
Token* CodeNode::getToken() {
  return token;
}

/**
 * @brief sets the token for this node
 * 
 */
void CodeNode::setToken(Token* t) {
  token = t;
}

/**
 * @brief sets the token for this node, copy version
 * 
 */
void CodeNode::setToken(const Token& t) {
  token = new Token(t);
}

/**
 * @brief gets the child of this node
 * 
 * @return CodeNode* of the child, probably nullptr
 */
CodeNode *CodeNode::getChild() {
  return child;
}

/**
 * @brief get the sibling of this node
 * 
 * @return CodeNode* of the sibling, or maaaaybe a nullptr
 */
CodeNode *CodeNode::getSibling() {
  return sibling;
}

/**
 * @brief set the child of this node
 * 
 * @param c new child pointer
 */
void CodeNode::setChild(CodeNode *c) {
  child = c;
}

/**
 * @brief set the sibling of this node
 * 
 * @param s new sibling pointer
 */
void CodeNode::setSibling(CodeNode *s) {
  sibling = s;
}

bool DescentParser::parse(const string& fname, CodeNode*& root, CodeScope*& table) {
  
  list<Token> tokenList;
  {
    Scanner s(fname);
    tokenList = s.getTokens();
  }

  if (tokenList.empty())  return false;

  root = nullptr;
  CodeNode* tail = nullptr;
  
  table = new CodeScope();
  CodeScope* curTable = table;

  Symbol* curSymbol = nullptr;

  enum class ss { //states for handling tokens
    START,
    SEEN_IDENTIFIER,
    VARIABLE_NAME,
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

  enum class fs { //states for the for-loop handling state machine
    NONE,
    EXPECTING_FIRST_SEMI,
    EXPECTING_SECOND_SEMI,
  };
  
  ss state = ss::START;
  fs forState = fs::NONE;

  for (auto t = tokenList.begin(); t != tokenList.end(); ++t) {
    auto nt = next(t);

    CodeNode* thisNode = new CodeNode(*t);

    bool descend;

    //figure out which direction we're going next
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

    //get lowercase spelling for token, we'll need it a lot
    const string lspell = getLowercase(t->getSpelling());

    //handle descents and ascents
    if (t->getType() == TokenType::LEFT_BRACE) {
      curTable = curTable->creatSubScope();
    }
    else if (t->getType() == TokenType::RIGHT_BRACE) {
      curTable = curTable->getParentScope();
    }

    //take action for this token
    switch (state) {
      case ss::START:
        switch (t->getType()) {
          case TokenType::IDENTIFIER:
            if (lspell == "function") {
              curSymbol = new Symbol();
              curSymbol->type = SymbolType::FUNCTION;
              state = ss::FUNC_RETURN_TYPE;
            }
            else if (lspell == "procedure") {
              curSymbol = new Symbol();
              curSymbol->type = SymbolType::PROCEDURE;
              state = ss::FUNC_NAME;
            }
            else if (lspell == "return") {
              state = ss::IGNORE;
            }
            else if (lspell == "for") {
              forState = fs::EXPECTING_FIRST_SEMI;
            }
            else {
              //this might be a variable declaration
              //if it is, the next token should also be an identifier
              //look ahead
              if (nt != tokenList.end() && nt->getType() == TokenType::IDENTIFIER) {
                curSymbol = new Symbol;
                curSymbol->type = typemap[getLowercase(t->getSpelling())];

                state = ss::VARIABLE_NAME;
              }

              else state = ss::SEEN_IDENTIFIER;
            }
          break;
          default:
            //if there's more to this statement, we really don't care
            state = ss::IGNORE;
        }
      break;

      case ss::VARIABLE_NAME: {
        curSymbol->name = t->getSpelling();
        
        state = ss::DECLARED_VARIABLE;
      }
      break;

      case ss::SEEN_IDENTIFIER:
        switch (t->getType()) {
          default:
            state = ss::IGNORE;
        }
      break;

      case ss::DECLARED_VARIABLE:
        switch (t->getType()) {
          case TokenType::LEFT_BRACKET:
            state = ss::ARRAY_DECLARATION;
            curSymbol->isArray = true;
          break;
          case TokenType::COMMA: {
            //make a note of the last type, cause it's the next one too
            SymbolType temp = curSymbol->type;
            //save this one
            string err = curTable->addSymbol(curSymbol);
            if (!err.empty()) {
              delete root;
              delete table;
              root = nullptr;
              table = nullptr;
              cout << "Error on line " << t->getLine() << ": " << err << endl;
              return false;
            };

            //make the new symbol for what comes after this comma
            curSymbol = new Symbol;
            curSymbol->type = temp;

            state = ss::VARIABLE_NAME;
          }break;
          default:
            string err = curTable->addSymbol(curSymbol);
            if (!err.empty()) {
              delete root;
              delete table;
              root = nullptr;
              table = nullptr;
              cout << "Error on line " << t->getLine() << ": " << err << endl;
              return false;
            };
            state = ss::IGNORE;
        }
      break;

      case ss::ARRAY_DECLARATION:
        switch (t->getType()) {
          case TokenType::INTEGER: {
            if (stoi(lspell) <= 0) {
              cout << "Syntax error on line " << t->getLine() << ": array declaration size must be a positive integer." << endl;
              exit(-1);
            }

            curSymbol->arraySize = stoi(t->getSpelling());
            string err = curTable->addSymbol(curSymbol);
            if (!err.empty()) {
              delete root;
              delete table;
              root = nullptr;
              table = nullptr;
              cout << "Error on line " << t->getLine() << ": " << err << endl;
              return false;
            };

            //now show me a close bracket
            if (nt == tokenList.end() || nt->getType() != TokenType::RIGHT_BRACKET ) {
              cerr << "Unexpected token " << nt->getSpelling() << " follows array declaration" << endl;
              exit(-1);
            }

            nt = next(nt);

            //ok is this a comma now?
            if (nt != tokenList.end() && nt->getType() == TokenType::COMMA ) {
              
              //make a note of this symbol, cause the next one is of the same type
              SymbolType temp = curSymbol->type;
              
              //save this one
              string err = curTable->addSymbol(curSymbol);
              if (!err.empty()) {
                delete root;
                delete table;
                root = nullptr;
                table = nullptr;
                cout << "Error on line " << t->getLine() << ": " << err << endl;
                return false;
              };

              //make the new symbol for what comes after this comma
              curSymbol = new Symbol;
              curSymbol->type =curSymbol->type;

              state = ss::VARIABLE_NAME;

              ++t;
            }
            else state = ss::IGNORE;
          }break;
          default:
            state = ss::IGNORE;
        }
      break;

      case ss::FUNC_RETURN_TYPE:
        curSymbol->returnType = typemap[getLowercase(t->getSpelling())];
        state = ss::FUNC_NAME;
      break;

      case ss::FUNC_NAME:
        switch(t->getType()) {
          case TokenType::IDENTIFIER:
            curSymbol->name = t->getSpelling();
            state = ss::FUNC_OPEN;
          break;
          default:
            state = ss::IGNORE;
        }
      break;

      case ss::FUNC_OPEN:{
        string err = curTable->addSymbolAndDescend(curSymbol, curTable);
        if (!err.empty()) {
          delete root;
          delete table;
          root = nullptr;
          table = nullptr;
          cout << "Error on line " << t->getLine() << ": " << err << endl;
          return false;
        }
        state = ss::FUNC_ARGTYPE;
      }break;

      case ss::FUNC_ARGTYPE:
        if (lspell == "void") {
          //eat the left brace
          while (++t != tokenList.end() && t->getType() != TokenType::LEFT_BRACE);
          state = ss::START;
        }
        else {
          curSymbol = new Symbol;
          curSymbol->type = typemap[getLowercase(t->getSpelling())];
          state = ss::FUNC_ARGNAME;
        }
      break;

      case ss::FUNC_ARGNAME:{
        curSymbol->name = t->getSpelling();

        //check if this parameter is an array
        if (nt != tokenList.end() && nt->getType() == TokenType::LEFT_BRACKET) {
          curSymbol->isArray = true;

          //increment nt to skip some tokens:
          nt = next(nt);  //skip bracket
          curSymbol->arraySize = stoi(nt->getSpelling());
          ++t;  //skip array size and right bracket
          ++t;
          ++t;
        }

        //record this argument
        string err = curTable->addParameter(curSymbol);
        if (!err.empty()) {
          delete root;
          delete table;
          root = nullptr;
          table = nullptr;
          cout << "Error on line " << t->getLine() << ": " << err << endl;
          return false;
        };

        state = ss::FUNC_COMMACLOSE;
    }break;

      case ss::FUNC_COMMACLOSE: {
        switch (t->getType()) {
          case TokenType::COMMA:
            state = ss::FUNC_ARGTYPE;
          break;
          case TokenType::RIGHT_PARENTHESIS:
            //eat the left brace
            while (++t != tokenList.end() && t->getType() != TokenType::LEFT_BRACE);
            state = ss::START;
          break;
          default: {
            cerr << "Unexpected token " << t->getSpelling() << " has terminated function definition";
            exit(-1);
          }
        }
      }break;

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

    if (root == nullptr) {
      root = thisNode;
      tail = root;
    }
    else if (descend) {
      if (tail == nullptr) {
        root->setChild(thisNode);
      }
      else {
        tail->setChild(thisNode);
      }
    }
    else {
      if (tail == nullptr) {
        root->setChild(thisNode);
      }
      else {
        tail->setSibling(thisNode);
      }
    }

    tail = thisNode;
  }

  return true;
}
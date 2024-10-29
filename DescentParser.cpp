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
  if (child)
    delete child;
  if (sibling)
    delete sibling;
  if (token)
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


/**
 * @brief Construct a new Descent Parser:: Descent Parser object. YOU'D BETTER CALL OPEN NEXT
 * 
 */
DescentParser::DescentParser() {
}

/**
 * @brief Construct a new Descent Parser:: Descent Parser object the safe way
 * 
 * @param fname filename to open for the underlying scanner object
 */
DescentParser::DescentParser(const string &fname) {
  open(fname);
}

/**
 * @brief Open a filename with the underlying scanner object
 * 
 * @param fname 
 */
void DescentParser::open(const string &fname) {
  s.open(fname);
}

/**
 * @brief where the magic happens. Creates a parse tree and error tests to be compliant with Assignment 3
 * 
 * @return CodeNode* the root of the parse tree
 */
CodeNode* DescentParser::parse() {
  list<Token> tokenList = s.getTokens();

  if (tokenList.empty())  return nullptr;

  descend = false;
  state = ss::START;
  forState = fs::NONE;

  return parse(tokenList.begin(), tokenList.end());
}

CodeNode* DescentParser::parse(const list<Token>::iterator& t, const list<Token>::iterator& end) {
  if (t == end) return nullptr;

  auto nt = next(t,1);

  CodeNode* thisNode = new CodeNode(*t);

  //figure out next branch
  if ((t->getType() == TokenType::SEMICOLON) ||
      (t->getType() == TokenType::LEFT_BRACE) ||
      (t->getType() == TokenType::RIGHT_BRACE) ||
        ((nt != end) &&
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
            cerr << "Syntax error on line " << t->getLine()
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
            cerr << "Syntax error on line " << t->getLine() << ": array declaration size must be a positive integer." << endl;
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
            cerr << "Syntax error on line " << t->getLine()
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
        cerr << "Syntax error on line " << t->getLine()
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

  if (descend) {
    thisNode->setChild(parse(nt, end));
  }
  else {
    thisNode->setSibling(parse(nt, end));
  }

  return thisNode;
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

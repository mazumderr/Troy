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
#include "PostFixEvaluator.hpp"

bool DescentParser::parse(const string& fname, SyntaxTree*& root, CodeScope*& table) {
  list<Token> tokenList;
  {
    Scanner s(fname);
    tokenList = s.getTokens();
  }

  if (tokenList.empty())  return false;

  root = new SyntaxTree;
  
  table = new CodeScope();
  CodeScope* curTable = table;

  for (auto t = tokenList.begin(); t != tokenList.end(); ++t) {
    // cout << "token: " << t->getSpelling() << endl;

    //get lowercase spelling for token, we'll need it a lot
    string lspell = getLowercase(t->getSpelling());

    //take action for this token
    switch (t->getType()) {
      case TokenType::IDENTIFIER: {
        //function
        if (lspell == "function") {
          //skip the word function
          //grab the return type
          string rType = (++t)->getSpelling(); 
          //grab the function name
          string fName = (++t)->getSpelling();
          ++t;

          //make a declaration code node
          CodeNode *n = new CodeNode;
          n->setType(NodeType::DECLARATION);
          
          //make a symbol for this function
          Symbol *s = new Symbol;
          s->name = fName;
          s->returnType = typemap[rType];

          //link the symbol to this code node
          s->myDeclaration = n;

          string err = curTable->addSymbolAndDescend(s, curTable);
          if (!err.empty()) {
            cout << "DescentParser: " << err << endl;
            return false;
          }

          err = getParameters(t, curTable);
          if (!err.empty()) {
            cout << "DescentParser: " << err << endl;
            return false;
          }

          root->descend(n);

          ++t;
          n = new CodeNode;
          n->setType(NodeType::BEGIN);
          n->setScope(curTable);

          root->descend(n);
        }
        //procedure
        else if (lspell == "procedure") {
          //grab the procedure name
          string pName = (++t)->getSpelling();
          ++t;

          //make a declaration code node
          CodeNode *n = new CodeNode;
          n->setType(NodeType::DECLARATION);

          //make a symbol for this procedure
          Symbol *s = new Symbol;
          s->name = pName;

          //link the symbol to this code node
          s->myDeclaration = n;
          
          string err = curTable->addSymbolAndDescend(s, curTable);
          if (!err.empty()) {
            cout << "DescentParser: " << err << endl;
            return false;
          }
          // cout << "p:"<<t->getSpelling() << endl;
          
          //grab each parameter for the subscope
          err = getParameters(t,curTable);
          if (!err.empty()) {
            cout << "DescentParser: " << err << endl;
            return false;
          }

          root->descend(n);

          // cout << "p:"<<t->getSpelling() << endl;
          ++t;
          n = new CodeNode;
          n->setType(NodeType::BEGIN);
          n->setScope(curTable);

          root->descend(n);
        }
        //if
        else if (lspell == "if") {
          //make a return code node
          CodeNode *n = new CodeNode;
          n->setType(NodeType::IF);

          PostFixEvaluator p;
          //run the evaluator one jump ahead so that we don't eat the open brace
          while (p.Eval(*next(t))) ++t;

          n->setExpression(p.getEquation());

          root->descend(n);
        }
        //for
        else if (lspell == "for") {
          CodeNode *n = new CodeNode;
          n->setType(NodeType::FOR_INITIAL);

          ++t;//skip over the word for
          ++t;//skip over the open parenthesis

          {
            vector<Token> expression = {*t};
            ++t; //skip over the variable to assign to
            ++t; //skip over the assignment operator
            PostFixEvaluator p;
            while (p.Eval(*t)) ++t;


            vector<Token> output = p.getEquation();
            expression.insert(expression.end(), output.begin(), output.end());
            n->setExpression(expression);

            // cout << "for initial: ";
            // for (auto t: expression) {
            //   cout << t.getSpelling() << " ";
            // }
            // cout << endl;
          }

          ++t;  //skip over the semicolon

          root->descend(n);

          n = new CodeNode;
          n->setType(NodeType::FOR_CONDITION);

          {
            PostFixEvaluator p;
            while (p.Eval(*t)) ++t;
            vector<Token> expression = p.getEquation();
            n->setExpression(expression);

            // cout << "for condition: ";
            // for (auto t: expression) {
            //   cout << t.getSpelling() << " ";
            // }
            // cout << endl;
          }

          ++t; // skip the second semicolon

          root->append(n);

          n = new CodeNode;
          n->setType(NodeType::FOR_INCREMENT);

          {
            vector<Token> expression = {*t};
            ++t; //skip over the variable to assign to
            ++t; //skip over the assignment operator
            PostFixEvaluator p;
            while (p.Eval(*t)) ++t;
            vector<Token> output = p.getEquation();
            expression.insert(expression.end(), output.begin(), output.end());
            n->setExpression(expression);

            // cout << "for increment: ";
            // for (auto t: expression) {
            //   cout << t.getSpelling() << " ";
            // }
            // cout << endl;
          }

          root->append(n);
        }
        //return
        else if (lspell == "return") {
          //make a return code node
          CodeNode *n = new CodeNode;
          n->setType(NodeType::RETURN);

          //attach the expression that is being returned
          PostFixEvaluator p;

          while (p.Eval(*(++t)));

          n->setExpression(p.getEquation());
          root->descend(n);
        }
        else if (lspell == "else") {
          //make a return code node
          CodeNode *n = new CodeNode;
          n->setType(NodeType::ELSE);
          root->descend(n);
        }
        //printf
        else if (lspell == "printf") {
          //make a return code node
          CodeNode *n = new CodeNode;
          n->setType(NodeType::PRINTF);

          ++t; //skip over the word printf
          ++t; //skip over the left parenthesis

          vector<Token> expression;

          while (t->getType() != TokenType::RIGHT_PARENTHESIS) {
            switch (t->getType()) {
              case TokenType::DOUBLE_QUOTE:
              case TokenType::SINGLE_QUOTE:
              case TokenType::COMMA:
                ++t;
                continue;
              break;
              default:{};
            }
            // cout << "param " << t->getSpelling() << endl;
            expression.push_back(*t);
            ++t;
          }
          
          n->setExpression(expression);

          ++t;

          root->descend(n);
        }
        //while
        else if (lspell == "while") {
          CodeNode *n = new CodeNode;
          n->setType(NodeType::WHILE);

          PostFixEvaluator p;
          //run the evaluator one jump ahead so that we don't eat the open brace
          while (p.Eval(*next(t))) ++t;

          n->setExpression(p.getEquation());

          root->descend(n);
        }
        //function call
        else if (next(t)->getType() == TokenType::LEFT_PARENTHESIS) {
          //make a return code node
          CodeNode *n = new CodeNode;
          n->setType(NodeType::CALL);

          
          vector<Token> expression = {*t};

          ++t; //skip over the function name
          ++t; //skip over the left parenthesis

          while (t->getType() != TokenType::RIGHT_PARENTHESIS) {
            switch (t->getType()) {
              case TokenType::DOUBLE_QUOTE:
              case TokenType::SINGLE_QUOTE:
              case TokenType::COMMA:
                ++t;
                continue;
              break;
              default:{};
            }
            // cout << "param " << t->getSpelling() << endl;
            expression.push_back(*t);
            ++t;
          }
          
          n->setExpression(expression);

          ++t;

          root->descend(n);
        }
        //assignment
        else if (next(t)->getType() == TokenType::ASSIGNMENT || next(t)->getType() == TokenType::LEFT_BRACKET) {
          //make an assignment node
          CodeNode *n = new CodeNode;
          n->setType(NodeType::ASSIGNMENT);
          vector<Token> expression = {*t};  //get the name we're assigning to
          ++t;//skip the name
          while (t->getType() != TokenType::ASSIGNMENT) {
            expression.push_back(*t);
            ++t;
          }
          ++t;//skip the equals
          PostFixEvaluator p;
          while (p.Eval(*t)) t++;
          vector<Token> output = p.getEquation();
          expression.insert(expression.end(), output.begin(), output.end());
          n->setExpression(expression);
          root->descend(n);
        }
        //variable declaration
        else if (next(t)->getType() == TokenType::IDENTIFIER) {
          //make a declaration code node
          CodeNode *n = new CodeNode;
          n->setType(NodeType::DECLARATION);
          
          //make a symbol for this function
          string vType = t->getSpelling();

          do {
            ++t; //skip over type name, or comma
            Symbol *s = new Symbol;
            string vName = t->getSpelling();
            ++t;
            s->name = vName;
            s->type = typemap[vType];
            handleArrayDeclaration(t, s);

            // cout << vType << " " << vName << (s->isArray ? " (array)" : "") << endl;

            string err = curTable->addSymbol(s);
            if (!err.empty()) {
              cout << "DescentParser: " << err << endl;
              return false;
            }
          } while (t->getType() == TokenType::COMMA);

          root->descend(n);
        }
      }break;
      case TokenType::LEFT_BRACE: {
        //make a begin code node
        CodeNode *n = new CodeNode;
        n->setType(NodeType::BEGIN);
        curTable = curTable->createSubScope(); 
        n->setScope(curTable);
        root->descend(n);
      }break;
      case TokenType::RIGHT_BRACE: {
        //make an end code node
        CodeNode *n = new CodeNode;
        n->setType(NodeType::END);
        root->descend(n);
        curTable = curTable->getParentScope();
      }break;
      
    }
  }

  return true;
}

void DescentParser::handleArrayDeclaration(list<Token>::iterator& t, Symbol* s) {
  //t at this point MUST be the first symbol after the identifier's name
  if (t->getType() == TokenType::LEFT_BRACKET) {
    s->isArray = true;
    s->arraySize = stoi((++t)->getSpelling());  //skip bracket, get size
    ++t;  //skip size
    ++t;  //skip close bracket
  }
}

string DescentParser::getParameters(list<Token>::iterator& t, CodeScope *& curTable) {
  //grab each parameter for the subscope
  //t at this point MUST point to the first symbol after our left parenthesis
  while (t->getType() != TokenType::RIGHT_PARENTHESIS) {
    ++t;
    string aType = t->getSpelling();
    if (getLowercase(aType) == "void") {
      ++t;  //skip the word void
      break;
    }
    string aName = (++t)->getSpelling();
    Symbol *a = new Symbol;
    a->name = aName;
    a->type = typemap[aType];

    ++t;
    handleArrayDeclaration(t, a);

    string err = curTable->addParameter(a);
    if (!err.empty()) return err;
  }

  return "";
}
#ifndef DESCENTPARSER_HPP
#define DESCENTPARSER_HPP

#include "Scanner/Scanner.hpp"

class CodeTree {
  public:
    CodeTree(const Token& t);

    Token* getToken();
    CodeTree* getChild();
    CodeTree* getSibling();
    void setChild(CodeTree*);
    void setSibling(CodeTree*);

  private:
    CodeTree* child = nullptr;
    CodeTree* sibling = nullptr;
    Token* token = nullptr;
};

class DescentParser {
  public:
    DescentParser();
    DescentParser(const string&);
    void open(const string&);

    CodeTree* parse();

  private:
    Scanner s;
    string getLowercase(const string&);
    bool wordIsForbidden(const string&);
};

#endif
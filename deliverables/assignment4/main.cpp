/**
 * @file main.cpp
 * @author Bo Thompson (extralife.xyz)
 * @brief deliverable for the symbol table
 * @version 0.1
 * @date 2024-10-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "../../DescentParser.hpp"

using namespace std;

int main(int argc, char** argv){

  if (argc < 2) {
    cout << "usage: " << argv[0] << " inputfile" << endl;
    return 0;
  }

  //get tokens
  {
    DescentParser d(argv[1]);
    d.parse();
    if (!d.checkError()) {

      list<Symbol*> symbolTable = d.getSymbolTable();

      list<Symbol*> functions;

      for (auto s: symbolTable) {
        s->print(true);
        
        //get pointers to all the function symbols so that we can print parameter lists later
        if (s->type == SymbolType::FUNCTION) functions.push_back(s);
      }

      cout << endl;

      for (auto f: functions) {
        cout << "   PARAMETER LIST FOR: " << f->name << endl;

        for (auto s: *(f->arguments)) {
          s->print(false);
        }
      }
    }
  }

  //here's the actual output for this file
  // cout << testout.str();

  return 0;
}

//a.exe io\in\assignment2\programming_assignment_2-test_file_1.c

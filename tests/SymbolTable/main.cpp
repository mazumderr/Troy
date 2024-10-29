/**
 * @file main.cpp
 * @author Bo Thompson (extralife.xyz)
 * @brief for testing the Symbol Table module
 * @version 0.1
 * @date 2024-10-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "../../DescentParser.hpp"

#include <sstream>
#include <fstream>

using namespace std;

int main(){

  string testIns[] = {
    "io/in/assignment4/programming_assignment_4-test_file_1.c",
    "io/in/assignment4/programming_assignment_4-test_file_2.c",
    "io/in/assignment4/programming_assignment_4-test_file_3.c",
    "io/in/assignment4/programming_assignment_4-test_file_4.c",
    "io/in/assignment4/programming_assignment_4-test_file_5.c",
    "io/in/assignment4/programming_assignment_4-test_file_6.c",
    "io/in/assignment4/programming_assignment_4-test_file_7.c",
  };

  string testOuts[] = {
    "io/out/assignment4/output-programming_assignment_4-test_file_1.txt",
    "io/out/assignment4/output-programming_assignment_4-test_file_2.txt",
    "io/out/assignment4/output-programming_assignment_4-test_file_3.txt",
    "io/out/assignment4/output-programming_assignment_4-test_file_4.txt",
    "io/out/assignment4/output-programming_assignment_4-test_file_5.txt",
    "io/out/assignment4/output-programming_assignment_4-test_file_6.txt",
    "io/out/assignment4/output-programming_assignment_4-test_file_7.txt",
  };

  //iterate across input files
  for (int i = 0; i < 7; ++i) {
    //announce the file we've made it to
    cout << "input file: " << testIns[i] << endl;
    cout << "key file: " << testOuts[i] << endl;

    //open the file containing the correct answers
    ifstream key(testOuts[i]);
    if (!key.is_open()) {
      cout << "unable to open key file " << testOuts[i] << endl;
      exit(-1);
    }

    //dump all output to a stream for comparison later
    stringstream testout;
    streambuf* coutBackup = cout.rdbuf(testout.rdbuf());

    //get tokens
    {
      DescentParser d(testIns[i]);
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

    //restore cout
    cout.rdbuf(coutBackup);

    //here's the actual output for this file
    // cout << testout.str();

    //verification
    {
      char c1,c2;
      unsigned int pos = 0, line = 0;

      while (testout.get(c1) && key.get(c2)) {
        if (tolower(c1) != tolower(c2)) {
          cout << "mismatch at " << line << ":" << pos << endl;

          string badline;
          testout.seekg(0);
          for (unsigned int i = 0; i <= line; ++i) {
            getline(testout,badline);
          }

          cout << "\"" << badline << "\"" << endl;

          cout << testout.str();

          exit(-1);
        }

        ++pos;
        if (c1 == '\n') {
          pos = 0;
          ++line;
        }
      }

      cout << "\tsuccess" << endl;
    }
  }

  return 0;
}

//a.exe io\in\assignment2\programming_assignment_2-test_file_1.c

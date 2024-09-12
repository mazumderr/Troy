/**
 * @file main.cpp
 * @author Bo Thompson (extralife.xyz)
 * @brief This version of main.cpp is for testing SourceReader's accuracy
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include "../../SourceReader.h"

using namespace std;

int main(){
  const string testInputs[] = {
    "io/in/assignment1/programming_assignment_1-test_file_1.c",
    "io/in/assignment1/programming_assignment_1-test_file_2.c",
    "io/in/assignment1/programming_assignment_1-test_file_3.c",
    "io/in/assignment1/programming_assignment_1-test_file_4.c",
    "io/in/assignment1/programming_assignment_1-test_file_5.c",
    "io/in/assignment1/programming_assignment_1-test_file_6.c",
    "io/in/assignment1/programming_assignment_1-test_file_7.c",
  };
  const string testOutputs[] = {
    "io/out/assignment1/programming_assignment_1-test_file_1-comments_replaced_with_whitespace.c",
    "io/out/assignment1/programming_assignment_1-test_file_2-comments_replaced_with_whitespace.c",
    "io/out/assignment1/programming_assignment_1-test_file_3-comments_replaced_with_whitespace.c",
    "io/out/assignment1/programming_assignment_1-test_file_4-comments_replaced_with_whitespace.c",
    "io/out/assignment1/5.c",
    "io/out/assignment1/6.c",
    "io/out/assignment1/7.c",
  };

  //iterate across all input files
  for (int i = 0; i < 7; ++i) {
    //open answer key
    ifstream key(testOutputs[i]);

    //open source file
    SourceReader s(testInputs[i]);


    //if the answer key actually exists
    if (!key.is_open()) {
      cerr << "Catastrophic error: cannot open \"answer key\" file "  << testOutputs[i] << endl;
      exit(-1);
    }

    char c1;
    char c2;
    unsigned int line = 0;

    stringstream buff;
    stringstream errBuff;

    streambuf* cerrBackup = cerr.rdbuf(errBuff.rdbuf());

    while (s.processSource(c1)) buff << string{c1};

    cerr.rdbuf(cerrBackup);

    stringstream* buffPtr = &buff;
    if (errBuff.rdbuf()->in_avail()) {
      buffPtr = &errBuff;
      cout << errBuff.str() << endl;
    }

    //compare character for character
    while (buffPtr->get(c1) && key.get(c2)) {

      //complain about mismatches
      if (c1 != c2) {
        cout << "error comparing" << endl << testInputs[i] << endl << "against" << endl << testOutputs[i] << endl
          << "at line " << line << endl;

        cout << "got \"" << string{c1} << "\", expected \"" << string{c2} << "\"\n";

        exit(-1);
      }
      
      if (c1 == '\n') line++;
    }

    //trumpet our successes
    cout << "file " << testInputs[i] << endl
      << " perfectly matches key " << testOutputs[i] << endl;
    
  }

  return 0;
}

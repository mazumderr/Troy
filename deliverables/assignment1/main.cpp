/**
 * @file main.cpp
 * @author Bo Thompson (extralife.xyz)
 * @brief This version of main.cpp is specifically for satisfying the requirements of assignment 1
 * @version 0.1
 * @date 2024-09-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <iostream>
#include "../../SourceReader.h"

using namespace std;

int main(int argc, char** argv){

  if (argc < 2) {
    cout << "usage: " << argv[0] << " inputfile" << endl;
    return 0;
  }

  //okay now assume that argv[1] is the desired input filename :D
  SourceReader s(argv[1]);

  char c;
  while (s.processSource(c)) {
    cout << c;
  }

  return 0;
}
#include "SourceReader.h"
#include <iostream>

/**
 * @brief Construct a new Source Reader:: Source Reader object
 * 
 * @pre input file must exist or an error is triggered
 * @param fname filename to read source code from
 */
SourceReader::SourceReader(const string &fname) {
  sourceFile.open(fname);

  if (!sourceFile.is_open()) {
    error("File \"" + fname + "\" does not exist.");
  }
}

/**
 * @brief (PLACEHOLDER) Get a line of source code, comments excluded
 * 
 * @pre c-style block comments must be correctly formatted or an error will be triggered
 * @param output a single line of commentless code
 * @return true if there's more to read
 * @return false if we're EOF
 */
bool SourceReader::getline(string &output){
  return false;
}

/**
 * @brief Prints and error and ends the program
 * 
 * @param msg error to print
 */
void SourceReader::error(string msg) {
  cout << msg << endl;
  exit(-1);
}

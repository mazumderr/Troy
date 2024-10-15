#include <iostream>
#include "../../DescentParser.hpp"

using namespace std;

int main(int argc, char** argv){

  if (argc < 2) {
    cout << "usage: " << argv[0] << " inputfile" << endl;
    return 0;
  }

  //okay now assume that argv[1] is the desired input filename :D
  CodeTree* t;
  {
    DescentParser d(argv[1]);
    t = d.parse();
  }

  CodeTree* ptr = t;
  unsigned int tab = 0;
  while (ptr != nullptr) {
    cout << getReadableTokenType(ptr->getToken()->getType()) << "\""
      << ptr->getToken()->getSpelling() << "\"";

    CodeTree* sibling = ptr->getSibling();
    CodeTree* child = ptr->getChild();
    if (sibling) {
      ++tab;
      cout << "\t";
      ptr = sibling;
    }
    else {
      cout << endl;
      for (unsigned int i = 0; i < tab; ++i) {
        cout << "\t";
      }
      ptr = child;
    }
  }
  
  return 0;
}
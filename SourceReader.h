#include <fstream>

using namespace std;

class SourceReader {
  public:
    SourceReader(const string& fname);
    bool getline(string& output);
  private:
    ifstream sourceFile;
    unsigned int curLine = 0;
    void error(string msg);
};
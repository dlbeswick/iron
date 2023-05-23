#include "parserphonepad/parserphonepad.h"
#include "parserphonepad/result.h"
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
  if (argc < 2) {
    cerr << "Usage: iron.exe input" << endl << endl;
    cerr << "Parameters:" << endl;
    cerr << "input       A string in 'phone pad' format to be parsed" << endl;
    return 1;
  }

  const Result<string> result = ParserPhonePad().parse(argv[1]);

  if (!result.error.empty()) {
    cerr << "An error occurred: " << result.error << endl;
    return 1;
  }
  else {
    cout << result.result() << endl;
    return 0;
  }
}

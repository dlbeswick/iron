#pragma once

#include <cassert>
#include <string>
#include <vector>
#include "parserphonepad/result.h"

/// <summary>
/// Parses strings that encode a series of button-presses on an old Nokia-style phone keypad.
///
/// Please note the following:
/// * The string must end with the terminator character '#'.
/// * There must be only one terminator in the string.
/// * The string must be non-empty.
/// * The string may only contain characters { 0..9, [space], *, & } 
///
/// Please also note that this simple class is less suitable for processing very large inputs as it stores the result as its
/// parsed, rather than streaming it out.
/// </summary>
class ParserPhonePad {

public:
  ParserPhonePad();

  /// <summary>
  /// Parse the input and return the character string that results.
  /// 
  /// </summary>
  Result<std::string> parse(const std::string& input) const;
   
private:
  const std::vector<std::vector<char>> keyMap;

  // A note on datatypes.
  //
  // A hashset is often a more appropriate data structure when the data is to be used for checking if a
  // particular value exists. However, for small datasets I believe a set may actually perform the same
  // or worse than a vector on x86 architectures post the year 2000. This is because a vector uses a
  // contiguous memory region and can exploit cache coherency for improved performance during its scan.
  //
  // This also depends on the value being stored -- whether calculation of a hash value followed by a
  // lookup and possible comparisons is faster than repeated comparisons. Comparisons of chars is very
  // fast.
  const std::vector<char> validInputs;

  const std::vector<char>& keypadToKeyMap(char key) const;
  char getCharForPresses(char keyChar, int keyPresses) const;
};
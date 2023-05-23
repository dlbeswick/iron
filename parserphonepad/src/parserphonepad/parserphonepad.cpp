#include "parserphonepad/parserphonepad.h"
using namespace std;

// Note: Defining this method outside of the class definition means they can be omitted from the
// header. It's appropriate as it's an implementation detail of the class and it accesses no state
// or other methods from the class.

// Also, a note on static data.
//
// This data could be defined statically, but I prefer not to avoid static data where possible, as
// any issues that occur during static initialization can be confusing and difficult to debug.
// If the goal is to avoid repeated allocation/calculation of this data, then it would be better to
// define an application-wide Parser instance (or a set of thread-local instances, as appropriate)
// and pass it around. This also avoids other unnecessary constructor calls.

// The indices of this vector represent keypad buttons. I.e. index '0' contains the characters assigned
// to the keypad with '0' printed on it, and so on.
static vector<vector<char>> initKeyMap() {
  return vector{
    vector { ' ' },
    vector { '&', '\'', '(', ')' }, // note: I have added an extra closing parenthesis here as it seemed appropriate.
    vector { 'A', 'B', 'C' },
    vector { 'D', 'E', 'F' },
    vector { 'G', 'H', 'I' },
    vector { 'J', 'K', 'L' },
    vector { 'M', 'N', 'O' },
    vector { 'P', 'Q', 'R', 'S' },
    vector { 'T', 'U', 'V' },
    vector { 'W', 'X', 'Y', 'Z' }
  };
};

// Note: using the initializer list for these members allows us to keep their definition const.
ParserPhonePad::ParserPhonePad() :
  keyMap(initKeyMap()),
  validInputs(vector{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '#', ' ', '*' })
{};

// A note on returning values rather than using non-const references as "output" parameters:
//
// Every major compiler should now implement the "named return value optimization", which will avoid
// copying the string on return in these cases. I think it's well worth using this method for clarity
// and simplicity.
Result<string> ParserPhonePad::parse(const string& input) const {
  if (input.empty()) {
    return Result<string>::Error("An empty string is not valid input");
  }
  
  // Parser state indicating which keypad key the user is currently 'pressing,' or -1 for no press yet.
  char keyPressing = -1;

  // Parser state indicating number of times 'keyPressing' has been pressed.
  int keyIterations = 0;
  string result;

  auto i = input.cbegin();
    
  for (; i != input.cend(); ++i) {
    if (find(validInputs.begin(), validInputs.end(), *i) == validInputs.end()) {
      return Result<string>::Error(string("Invalid input: ") + *i);
    }
      
    if (*i == '*') {
      // The asterisk has been 'pressed.'
      // Two scenarios can occur:
      //
      // 1. The user has started typing a character. The character input should be cancelled and state restarted.
      // 2. No character has been started. The last entered input should be removed from the result string.
      keyPressing = -1;
        
      if (keyIterations == 0 && !result.empty())
        result.pop_back();
        
      keyIterations = 0;
    } else if (*i == ' ') {
      // The user has requested a 'wait'. This writes out the result of any current keypress sequence and resets the sequence state.
      
      // This will be true when multiple 'waits' are present in the input string.
      if (keyPressing != -1)
        result += getCharForPresses(keyPressing, keyIterations);

      keyPressing = -1;
      keyIterations = 0;
    } else if (keyPressing == -1) {
      // The user has begin a new sequence of one or more presses on a keypad.
      keyPressing = *i;
      ++keyIterations;
    } else if (*i == keyPressing) {
      // The user has pressed the same keypad button repeatedly.
      ++keyIterations;
    } else {
      // The user has pressed another button.
      assert(keyPressing != -1 && *i != ' ');

      // This will be true if multiple 'waits' are present in the string.
      result += getCharForPresses(keyPressing, keyIterations);
        
      keyPressing = *i;
      keyIterations = 1;
    }

    if (*i == '#')
      break;
  }

  if (i == input.end() || *i != '#') {
    return Result<string>::Error("End of input reached without terminating '#' token");
  }
    
  if (i != input.end() && (++i) != input.end()) {
    return Result<string>::Error("Unexpected input beyond terminating '#' token");
  }
    
  return Result<string>::Ok(result);
};

/// <summary>
/// Given a valid literal keypad character (i.e. '0', '1', etc.) return the index 
/// </summary>
/// <param name="key">A literal character from '0' to '9'</param>
/// <returns></returns>
const vector<char>& ParserPhonePad::keypadToKeyMap(char key) const {
  // Only a programming error could make it possible for this assert to fail.
  // Otherwise, valid input is ensured by previous code steps.
  assert(key >= '0' && key <= '9');
    
  return keyMap[size_t(key) - '0'];
};

/// <summary>
/// Given a literal keypad character and a number of times the button having that character was pressed,
/// returns the resulting output character.
///
/// Note that repeated presses of the same button will 'wrap around' to the beginning of the character
/// list for that button.
/// </summary>
/// <param name="keyChar">A valid, literal keypad symbol, i.e. '0', '1', etc.</param>
/// <param name="keyPresses">The number of times the key was pressed.</param>
char ParserPhonePad::getCharForPresses(char keyChar, int keyPresses) const {
  auto charMap = keypadToKeyMap(keyChar);
  return charMap[(size_t(keyPresses) - 1) % charMap.size()];
};
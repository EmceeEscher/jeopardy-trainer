#include "Clue.h"
#include <string>

using std::string;
using namespace clue;

bool Clue::test_answer(string attempt) {
  return m_answer.compare(attempt) == 0;
}

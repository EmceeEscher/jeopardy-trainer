#include "Clue.h"
#include <string>

using std::string;
using namespace clue;

Clue::Clue(string clue, string answer, int value, bool is_daily_double) {
  m_clue = clue;
  m_answer = answer;
  m_value = value;
  m_is_daily_double = is_daily_double;
}

bool Clue::test_answer(string attempt) {
  return m_answer.compare(attempt) == 0;
}

#include "Clue.h"
#include <string>

using std::string;
using namespace clue;

Clue::Clue(std::string clue, std::string answer, int value) {
  m_clue = clue;
  m_answer = answer;
  m_value = value;
}

bool Clue::test_answer(std::string attempt) {
  return m_answer.compare(attempt) == 0;
}


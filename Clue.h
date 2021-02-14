#ifndef JEOPARDY_TRAINER__CLUE_H_
#define JEOPARDY_TRAINER__CLUE_H_

#include <string>

namespace clue {
  class Clue {
   private:
    std::string m_clue;
    std::string m_answer;
    int m_value;
    bool m_is_daily_double;

   public:
    Clue(std::string clue, std::string answer, int value, bool is_daily_double);

    bool test_answer(std::string attempt);

    std::string get_clue() { return m_clue; };
    int get_value() { return m_value; };
    bool get_is_daily_double() { return m_is_daily_double; };
  };
}

#endif //JEOPARDY_TRAINER__CLUE_H_

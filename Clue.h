#ifndef JEOPARDY_TRAINER__CLUE_H_
#define JEOPARDY_TRAINER__CLUE_H_
#include <string>

namespace clue {
  class Clue {
   private:
    std::string m_clue;
    std::string m_answer;
    int m_value;

   public:
    Clue(std::string clue, std::string answer, int value);

    bool test_answer(std::string attempt);
    std::string ask_question() { return m_clue; };
    int get_value() { return m_value; };
  };
}

#endif //JEOPARDY_TRAINER__CLUE_H_

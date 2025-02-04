#ifndef JEOPARDY_TRAINER__CLUE_H_
#define JEOPARDY_TRAINER__CLUE_H_

#include <string>
#include <vector>

namespace clue {
  struct Clue {
    std::string m_clue = "";
    std::string m_answer = "";
    std::vector<std::string> m_links;
    int m_value = 0;
    bool m_is_daily_double = false;
    bool m_is_final_jeopardy = false;
    std::string m_comments = "";

    // Returns whether you got the answer right
    bool test_answer(std::string attempt);
  };
}

#endif //JEOPARDY_TRAINER__CLUE_H_

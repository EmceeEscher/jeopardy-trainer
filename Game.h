#ifndef JEOPARDY_TRAINER__GAME_H_
#define JEOPARDY_TRAINER__GAME_H_

#include <vector>
#include <ctime>

#include "Clue.h"
#include "Category.h"

namespace game {
  struct Game {
    std::vector<category::Category> m_single_jeopardy;
    std::vector<category::Category> m_double_jeopardy;
    category::Category m_final_jeopardy;
    std::string m_air_date;
  };
}

#endif //JEOPARDY_TRAINER__GAME_H_

#ifndef JEOPARDY_TRAINER__GAME_H_
#define JEOPARDY_TRAINER__GAME_H_

#include <vector>
#include <ctime>

#include "Clue.h"
#include "Category.h"

namespace game {
  class Game {
   private:
    std::vector<category::Category> m_single_jeopardy;
    std::vector<category::Category> m_double_jeopardy;
    category::Category m_final_jeopardy;
    std::time_t m_air_date;

   public:
    // Constructor
    Game(
        std::vector<category::Category> &single_jeopardy,
        std::vector<category::Category> &double_jeopardy,
        category::Category final_jeopardy,
        std::time_t air_date
        );

    // Getters / setters
    std::vector<category::Category> const &get_single_jeopardy() { return m_single_jeopardy; };
    std::vector<category::Category> const &get_double_jeopardy() { return m_double_jeopardy; };
    category::Category const &get_final_jeopardy() { return m_final_jeopardy; };
    std::time_t get_air_date() { return m_air_date; };
  };
}

#endif //JEOPARDY_TRAINER__GAME_H_

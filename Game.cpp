#include "Game.h"

#include <vector>
#include <ctime>

using std::vector;
using std::time_t;
using namespace game;

Game::Game(
    vector<category::Category> &single_jeopardy,
    vector<category::Category> &double_jeopardy,
    category::Category final_jeopardy,
    time_t air_date)
    : m_final_jeopardy(final_jeopardy), m_air_date(air_date) {
  m_single_jeopardy = single_jeopardy;
  m_double_jeopardy = double_jeopardy;
}
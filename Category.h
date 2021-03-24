#ifndef JEOPARDY_TRAINER__CATEGORY_H_
#define JEOPARDY_TRAINER__CATEGORY_H_

#include <string>
#include <vector>
#include "Clue.h"

namespace category {
  enum Type {
    US_geography,
    world_geography,
    US_history,
    world_history,
    science,
    literature,
    film,
    music,
    theater,
    arts,
    vocabulary,
    wordplay,
    other,
    unknown
  };

  struct Category {
    std::string m_title;
    Type m_type = unknown;
    std::vector<clue::Clue> m_clues;
    bool m_is_double_jeopardy = false;
    bool m_is_final_jeopardy = false;
  };
}

#endif //JEOPARDY_TRAINER__CATEGORY_H_

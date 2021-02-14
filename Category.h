#ifndef JEOPARDY_TRAINER__CATEGORY_H_
#define JEOPARDY_TRAINER__CATEGORY_H_

#include <string>
#include "Clue.h"

namespace category {
  enum Type {
    wordplay,
    US_geography,
    world_geography,
    US_history,
    world_history,
    science,
    literature,
    film,
    vocabulary,
    other
  };

  class Category {
   private:
    std::string m_title;
    Type m_type;
    clue::Clue m_clues[6];
    bool m_is_double_jeopardy;

   public:
    std::string get_title() { return m_title; };
    Type get_type() { return m_type; };
    bool get_is_double_jeopardy() { return m_is_double_jeopardy; };

  };
}

#endif //JEOPARDY_TRAINER__CATEGORY_H_

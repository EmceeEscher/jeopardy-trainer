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
    vocabulary,
    wordplay,
    other,
    unknown
  };

  class Category {
   private:
    std::string m_title;
    Type m_type;
    std::vector<clue::Clue> m_clues;
    bool m_is_double_jeopardy;

   public:
    // Constructors
    Category(std::string title, std::vector<clue::Clue> &clues, bool is_double_jeopardy, Type type=unknown);

    // Getters / setters
    std::string get_title() { return m_title; };

    Type get_type() { return m_type; };
    void set_type(Type type) { m_type = type; };

    std::vector<clue::Clue> get_clues() { return m_clues; };

    bool get_is_double_jeopardy() { return m_is_double_jeopardy; };
  };
}

#endif //JEOPARDY_TRAINER__CATEGORY_H_

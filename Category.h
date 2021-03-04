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

  class Category {
   private:
    std::string m_title;
    Type m_type;
    std::vector<clue::Clue> m_clues;
    bool m_is_double_jeopardy;
    bool m_is_final_jeopardy;

   public:
    // Constructor
    Category(
        std::string title,
        std::vector<clue::Clue> &clues,
        bool is_double_jeopardy,
        bool is_final_jeopardy,
        Type type=unknown);

    // Getters / setters
    std::string get_title() { return m_title; };

    Type get_type() { return m_type; };
    void set_type(Type type) { m_type = type; };

    // TODO: I had this as const but then it made problems? Not sure what is correct
    std::vector<clue::Clue> &get_clues() { return m_clues; };

    bool get_is_double_jeopardy() { return m_is_double_jeopardy; };
    bool get_is_final_jeopardy() { return m_is_final_jeopardy; };
  };
}

#endif //JEOPARDY_TRAINER__CATEGORY_H_

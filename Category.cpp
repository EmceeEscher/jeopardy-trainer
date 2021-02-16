#include "Category.h"

using std::string;
using std::vector;
using namespace category;

Category::Category(string title, vector<clue::Clue> &clues, bool is_double_jeopardy, Type type) {
  m_title = title;
  m_type = type;
  m_is_double_jeopardy = is_double_jeopardy;

  // TODO: will this create a memory leak? not sure about the vector copying
  m_clues = clues;
}
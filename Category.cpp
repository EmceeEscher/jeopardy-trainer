#include "Category.h"

using std::string;
using std::vector;
using namespace category;

Category::Category(
    string title,
    vector<clue::Clue> &clues,
    bool is_double_jeopardy,
    bool is_final_jeopardy,
    Type type)
    : m_type(type), m_is_double_jeopardy(is_double_jeopardy), m_is_final_jeopardy(is_final_jeopardy){
  m_title = title;

  // TODO: will this create a memory leak? not sure about the vector copying
  m_clues = clues;
}
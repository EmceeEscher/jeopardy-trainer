#include <iostream>
#include <vector>
#include <string>

#include "Clue.h"
#include "Category.h"

using namespace clue;
using namespace category;

int main() {
  Clue test_clue("what this object is", "clue", 200, false, false);

  std::vector<Clue> all_test_clues;
  all_test_clues.push_back(test_clue);
  Category test_category("random facts", all_test_clues, false, false);

  std::cout << test_category.get_title() << std::endl;
  std::cout << test_category.get_clues().at(0).get_clue() << std::endl;
  return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <curl/curl.h>

#include "Clue.h"
#include "Category.h"
#include "WebParser.h"

using namespace clue;
using namespace category;
using namespace web_parser;

int main() {
  Clue test_clue("what this object is", "clue", 200, false, false);

  std::vector<Clue> all_test_clues;
  all_test_clues.push_back(test_clue);
  Category test_category("random facts", all_test_clues, false, false);

  std::cout << test_category.get_title() << std::endl;
  std::cout << test_category.get_clues().at(0).get_clue() << std::endl;

  WebParser parser = WebParser();

  parser.retrieve_web_page("https://www.j-archive.com/showgame.php?game_id=6699");
  return 0;
}

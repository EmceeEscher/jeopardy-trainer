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
  WebParser parser = WebParser();

  parser.retrieve_web_page("https://www.j-archive.com/showgame.php?game_id=6992");
  return 0;
}

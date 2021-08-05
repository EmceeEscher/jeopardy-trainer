#include <iostream>
#include <vector>
#include <string>
#include <curl/curl.h>
#include <algorithm>

#include "Clue.h"
#include "Category.h"
#include "Game.h"
#include "WebParser.h"
#include "DbHandler.h"

using namespace clue;
using namespace category;
using namespace web_parser;
using namespace game;
using std::string;

int main() {
  DbHandler db_handler = DbHandler();
  db_handler.setup_db_conn();

  WebParser parser = WebParser();

  Game game;
  // run 1: 173
  // run 2: 174-200, failed with segfault after completing 191
  // run 3: 192, fails with segfault! (so not a memory leak??)
  for (int i = 192; i <= 192; i++) {
    string url = string("https://www.j-archive.com/showgame.php?game_id=") + std::to_string(i);

  CURLcode parsed_page_code = parser.retrieve_web_page(url.c_str(), &game);
    if (parsed_page_code == CURLE_OK) {
      bool wrote_game = db_handler.write_full_game(game);
      if (!wrote_game) {
        std::cout << "failed to write to db for game " << i << std::endl;
        break;
      }
    } else {
      std::cout << "couldn't get page for id " << i << std::endl;
    }
  }

  db_handler.close_db_conn();
  return 0;
}

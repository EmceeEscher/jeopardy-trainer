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
  // got 192 working, but noticed it wasn't separating links, so fixed that and started over
  // run 4: 173-200, success
  // run 5: 201-1000, broke on 233 (guess is due to underline in category name)
  // run 6: fixed 233, time to continue
  // run 7: 234-300
  // run 8: 301-500
  // run 9: 501-1000, I think broke on 940?
  // run 10: fixed 940
  // run 11: 941-1000, success
  // run 12: realized 1-172 are valid, so running those
  // run 13: 1001-1500, failed on 1132, no data for 1132-1135 other than dates and contestants, so skipping them
  // run 14: 1136-1500, broke on 1153 (also no data), but noticed that 1152 had a tiebreaker round that wasn't covered
  // ... tiebreaks are rare, though, and I'm lazy, so... just gonna skip them
  // run 15: 1154-1500, failed on 1385
  // run 16: fixed 1385, but noticed error with category names with underlines (and no links, unlike 233)
  // manually fixed missing underline categories, next run should start with 1386
  for (int i = 1386; i <= 1500; i++) {
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

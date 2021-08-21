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
  // run 17: 1386-1500, worked
  // run 18: 1501-2000, broke on 1794
  // run 19: fixed 1794
  // run 20: 1795-2000, WORKED
  // run 21: 2001-2500, worked
  // run 22: 2501-3000, worked
  // run 23: 3001-3500, worked
  // run 24: 3501-4000, broke on 3575
  // run 25: added special casing for 3575 (only had one round)
  // run 26: also did 3576 with special casing
  // run 27: removed special casing, did 3577-4000, worked
  // run 28: 4001-4500, broke on 4246 due to same special casing
  // run 29: 4246
  // run 30: 4247-4500, broke on 4380 but too tired to fix rn
  // run 31: 4380 fixed
  // run 32: 4381-4500
  for (int i = 4381; i <= 4500; i++) {
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

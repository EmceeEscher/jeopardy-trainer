#include <iostream>
#include <vector>
#include <string>
#include <curl/curl.h>

#include "Clue.h"
#include "Category.h"
#include "Game.h"
#include "WebParser.h"
#include "DbHandler.h"

using namespace clue;
using namespace category;
using namespace web_parser;
using namespace game;

int main() {
  DbHandler db_handler = DbHandler();
  db_handler.setup_db_conn();

  WebParser parser = WebParser();

  Game game;
  parser.retrieve_web_page("https://www.j-archive.com/showgame.php?game_id=6989", &game);

  db_handler.write_full_game(game);
  db_handler.close_db_conn();
  return 0;
}

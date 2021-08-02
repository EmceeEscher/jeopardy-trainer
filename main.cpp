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
  db_handler.setupDbConn();

  WebParser parser = WebParser();

  Game game;
  parser.retrieve_web_page("https://www.j-archive.com/showgame.php?game_id=6989", &game);

  Clue fake_clue = game.m_double_jeopardy[2].m_clues[2];
  db_handler.writeClue(fake_clue);
  db_handler.closeDbConn();
  return 0;
}

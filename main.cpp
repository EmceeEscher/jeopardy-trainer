#include <iostream>
#include <vector>
#include <string>
#include <curl/curl.h>

#include "Clue.h"
#include "Category.h"
#include "WebParser.h"
#include "DbHandler.h"

using namespace clue;
using namespace category;
using namespace web_parser;

int main() {
  DbHandler db_handler = DbHandler();
  db_handler.setupDbConn();

  WebParser parser = WebParser();

  //TODO: refactor web parser to return the game somehow and write the actual clues
  parser.retrieve_web_page("https://www.j-archive.com/showgame.php?game_id=6989");

  Clue fake_clue;
  db_handler.writeClue(fake_clue);
  db_handler.closeDbConn();
  return 0;
}

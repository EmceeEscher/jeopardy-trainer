#ifndef JEOPARDY_TRAINER__DBHANDLER_H_
#define JEOPARDY_TRAINER__DBHANDLER_H_

#include <sqlite3.h>

#include "Clue.h"
#include "Category.h"
#include "Game.h"

class DbHandler {
 private:
  sqlite3 *m_conn;

  std::string bool_to_string(bool b);
  std::string escape_apostrophe(std::string original);
  std::string replace_all(std::string str, const std::string& from, const std::string& to);

 public:
  bool setup_db_conn();
  void close_db_conn();
  int write_clue(clue::Clue clue);
  int write_category(category::Category category);
  int write_game(game::Game game);
  bool write_category_clue(int category_id, int clue_id);
  bool write_game_category(int game_id, int category_id);
  bool write_all_categories_in_board(int game_id, std::string air_date, std::vector<category::Category> categories);
  bool write_full_game(game::Game game);
};

#endif //JEOPARDY_TRAINER__DBHANDLER_H_

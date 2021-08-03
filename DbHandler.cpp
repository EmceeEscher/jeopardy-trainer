#include "DbHandler.h"

#include <string>
#include <algorithm>

using clue::Clue;
using category::Category;
using game::Game;
using std::string;

bool DbHandler::setup_db_conn() {
  int res = sqlite3_open("/Users/jacob/sqlite-autoconf-3350500/jeopardy.db", &m_conn);

  if (res == SQLITE_OK) {
    return true;
  }

  sqlite3_close(m_conn);
  return false;
}

void DbHandler::close_db_conn() {
  if (m_conn) {
    sqlite3_close(m_conn);
  }
}

//TODO: clean up test clues in the db
// TODO: set up schema for categories/games, may need to update schema for associations too
int DbHandler::write_clue(Clue clue) {
  string insert_cmd_str = string("INSERT INTO clues (clue, answer, value, is_daily_double, is_final_jeopardy");
  if (!clue.m_comments.empty()) {
    insert_cmd_str += ", comments";
  }
  if (!clue.m_links.empty()) {
    insert_cmd_str += ", links";
  }

  insert_cmd_str += ") VALUES('"
      + escape_apostrophe(clue.m_clue) + "', '" + escape_apostrophe(clue.m_answer) + "', "
      + std::to_string(clue.m_value) + ", " + bool_to_string(clue.m_is_daily_double) + ", "
      + bool_to_string(clue.m_is_final_jeopardy);
  if (!clue.m_comments.empty()) {
    insert_cmd_str += ", '" + escape_apostrophe(clue.m_comments) + "'";
  }
  if (!clue.m_links.empty()) {
    string link_str;
    for (string &piece : clue.m_links) { link_str += piece; }
    insert_cmd_str += ", '" + link_str + "'";
  }

  insert_cmd_str += ")";

  sqlite3_stmt *stmt;
  const char *tail;

  int prep_result = sqlite3_prepare_v2(m_conn, insert_cmd_str.c_str(), -1, &stmt, &tail);
  if (prep_result != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return -1;
  }

  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  return sqlite3_last_insert_rowid(m_conn);
}

// TODO: update to also write type if I ever add types
int DbHandler::write_category(Category category) {
  string insert_cmd_str = string("INSERT INTO categories (title, is_double_jeopardy, is_final_jeopardy");
  if (!category.m_comments.empty()) {
    insert_cmd_str += ", comments";
  }
  if (!category.m_link.empty()) {
    insert_cmd_str += ", links";
  }

  insert_cmd_str += ") VALUES('"
      + escape_apostrophe(category.m_title) + "', " + bool_to_string(category.m_is_double_jeopardy) + ", "
      + bool_to_string(category.m_is_final_jeopardy);
  if (!category.m_comments.empty()) {
    insert_cmd_str += ", '" + escape_apostrophe(category.m_comments) + "'";
  }
  if (!category.m_comments.empty()) {
    insert_cmd_str += ", '" + escape_apostrophe(category.m_link) + "'";
  }

  insert_cmd_str += ")";

  sqlite3_stmt *stmt;
  const char *tail;

  int prep_result = sqlite3_prepare_v2(m_conn, insert_cmd_str.c_str(), -1, &stmt, &tail);
  if (prep_result != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return -1;
  }

  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  return sqlite3_last_insert_rowid(m_conn);
}

bool DbHandler::write_clue_category(int category_id, int clue_id) {
  string insert_cmd_str = string("INSERT INTO clue_categories (clue_id, category_id) VALUES(")
      + std::to_string(clue_id) + ", " + std::to_string(category_id) + ")";

  sqlite3_stmt *stmt;
  const char *tail;

  int prep_result = sqlite3_prepare_v2(m_conn, insert_cmd_str.c_str(), -1, &stmt, &tail);
  if (prep_result != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
  }

  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  return true;
}

bool DbHandler::write_full_game(game::Game game) {
  Category test_category = game.m_single_jeopardy[0];

  int category_id = write_category(test_category);
  for (Clue clue : test_category.m_clues) {
    int clue_id = write_clue(clue);
    write_clue_category(category_id, clue_id);
  }

  return true;
}

string DbHandler::bool_to_string(bool b) {
  return b ? "true" : "false";
}

string DbHandler::escape_apostrophe(std::string original) {
  string new_str = replace_all(original, string("'"), string("''"));
  return new_str;
}

// from https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
string DbHandler::replace_all(std::string str, const std::string &from, const std::string &to) {
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
  }
  return str;
}
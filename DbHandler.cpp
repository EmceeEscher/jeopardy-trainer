#include "DbHandler.h"

#include <string>
#include <algorithm>

using clue::Clue;
using std::string;

bool DbHandler::setupDbConn() {
  int res = sqlite3_open("/Users/jacob/sqlite-autoconf-3350500/jeopardy.db", &m_conn);

  if (res == SQLITE_OK) {
    return true;
  }

  sqlite3_close(m_conn);
  return false;
}

void DbHandler::closeDbConn() {
  if (m_conn) {
    sqlite3_close(m_conn);
  }
}

//TODO: clean up test clues in the db
// TODO: set up schema for categories/games, may need to update schema for associations too
bool DbHandler::writeClue(Clue clue) {
  string insert_cmd_str = string("INSERT INTO clues (clue, answer, value, is_daily_double, is_final_jeopardy");
  if (!clue.m_comments.empty()) {
    insert_cmd_str += ", comments";
  }
  if (!clue.m_links.empty()) {
    insert_cmd_str += ", links";
  }

  insert_cmd_str += ") VALUES('"
      + escapeApostrophe(clue.m_clue) + "', '" + escapeApostrophe(clue.m_answer) + "', "
      + std::to_string(clue.m_value) + ", " + boolToString(clue.m_is_daily_double) + ", "
      + boolToString(clue.m_is_final_jeopardy);
  if (!clue.m_comments.empty()) {
    insert_cmd_str += ", '" + escapeApostrophe(clue.m_comments) + "'";
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
    return false;
  }
  // TODO: return id so I can associate it with the category?
  sqlite3_step(stmt);
  return true;
}

string DbHandler::boolToString(bool b) {
  return b ? "true" : "false";
}

string DbHandler::escapeApostrophe(std::string original) {
  string new_str = replaceAll(original, string("'"), string("''"));
  return new_str;
}

// from https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
string DbHandler::replaceAll(std::string str, const std::string &from, const std::string &to) {
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
  }
  return str;
}
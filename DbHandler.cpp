#include "DbHandler.h"

#include <string>

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
bool DbHandler::writeClue(Clue clue) {
  string insert_cmd_str = string("INSERT INTO clues (clue, answer, value, is_daily_double, is_final_jeopardy) VALUES('")
      + clue.m_clue + "', '" + clue.m_answer + "', " + std::to_string(clue.m_value) + ", "
      + boolToString(clue.m_is_daily_double) + ", " + boolToString(clue.m_is_final_jeopardy) + ")";
  sqlite3_stmt *stmt;
  const char *tail;

  // Hopefully 800 will be high enough for nByte, might need to up for large clues
  sqlite3_prepare_v2(m_conn, insert_cmd_str.c_str(), 800, &stmt, &tail);
  //TODO: check for errors in result
  sqlite3_step(stmt);
  return true;
}

string DbHandler::boolToString(bool b) {
  return b ? "true" : "false";
}
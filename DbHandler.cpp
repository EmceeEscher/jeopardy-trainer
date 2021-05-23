#include "DbHandler.h"

using clue::Clue;

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
  //TODO write this so it actually uses the clue
  char *insert_cmd = "INSERT INTO clues (clue, answer, value, is_daily_double, is_final_jeopardy) VALUES('test clue', 'test answer', 200, true, false)";
  sqlite3_stmt *stmt;
  const char *tail;

  //TODO: figure out a good nByte value
  sqlite3_prepare_v2(m_conn, insert_cmd, 400/*??*/, &stmt, &tail);
  //TODO: check for errors in result
  sqlite3_step(stmt);
  return true;
}
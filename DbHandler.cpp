#include "DbHandler.h"

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
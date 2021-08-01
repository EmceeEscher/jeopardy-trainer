#ifndef JEOPARDY_TRAINER__DBHANDLER_H_
#define JEOPARDY_TRAINER__DBHANDLER_H_

#include <sqlite3.h>

#include "Clue.h"

class DbHandler {
 private:
  sqlite3 *m_conn;

  std::string boolToString(bool b);

 public:
  bool setupDbConn();
  void closeDbConn();
  bool writeClue(clue::Clue clue);
};

#endif //JEOPARDY_TRAINER__DBHANDLER_H_

#ifndef JEOPARDY_TRAINER__WEBPARSER_H_
#define JEOPARDY_TRAINER__WEBPARSER_H_

#include <curl/curl.h>
#include <string>

#include "Game.h"

namespace web_parser {
  CURLcode retrieve_web_page(const char *url);
  game::Game parse_game_page(CURLcode page_data);
}

#endif //JEOPARDY_TRAINER__WEBPARSER_H_

#ifndef JEOPARDY_TRAINER__WEBPARSER_H_
#define JEOPARDY_TRAINER__WEBPARSER_H_

#include <curl/curl.h>
#include <string>
#include <functional>
#include <libxml/HTMLparser.h>
#include <vector>

#include "Game.h"
#include "Clue.h"
#include "Category.h"

namespace web_parser {
  class WebParser {
   private:
    struct MemoryStruct {
      char *memory;
      size_t size;
    };
    static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userdata);
    static xmlNode *find_node(xmlNode *root_node, std::function<bool (xmlNode *)> search_func);
    static bool check_node(xmlNode *node, const char *node_type, const char *prop_type, const char *class_name);
    static bool is_clue_node(xmlNode *node);
    static bool is_category_node(xmlNode *node);
    static bool is_jeopardy_node(xmlNode *node);
    static bool is_double_jeopardy_node(xmlNode *node);
    static bool is_final_jeopardy_node(xmlNode *node);
    static bool is_title_node(xmlNode *node);
    static bool is_tr_node(xmlNode *node);
    static void parse_nodes(xmlNode *root_node, std::function<void (xmlNode *, void *)> parse_func, void *parse_struct);
    static void parse_clue_helper(xmlNode *node, void *clue_ptr);
    static std::string parse_string_helper(std::string full_str, std::string start_substr, std::string end_substr);
    clue::Clue parse_clue(xmlNode *clue_node);
    static void parse_category_name_helper(xmlNode *node, void *category_ptr);
    category::Category initialize_category(xmlNode *category_node, bool is_double_jeopardy, bool is_final_jeopardy);
    std::vector<category::Category> parse_round(xmlNode *round_node, bool is_double_jeopardy);
    category::Category parse_final_jeopardy(xmlNode *round_node);

   public:
    WebParser() = default;
    CURLcode retrieve_web_page(const char *url, game::Game *game_ptr);
    void parse_game_page(htmlDocPtr doc, game::Game *game_ptr);
  };
}

#endif //JEOPARDY_TRAINER__WEBPARSER_H_

#include "WebParser.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include <libxml/xmlsave.h>

using std::string;
using std::vector;
using game::Game;
using clue::Clue;
using category::Category;
using namespace web_parser;

// This function is taken almost verbatim from a libcurl tutorial: https://curl.se/libcurl/c/getinmemory.html
size_t WebParser::write_memory_callback(void *contents, size_t size, size_t nmemb, void *userdata) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userdata;

  char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

CURLcode WebParser::retrieve_web_page(const char *url) {

  CURL *curl_handle = curl_easy_init();

  if (curl_handle) {
    struct MemoryStruct page_mem_chunk;

    page_mem_chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc in the callback */
    page_mem_chunk.size = 0;    /* no data at this point */

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&page_mem_chunk);

    CURLcode res = curl_easy_perform(curl_handle);

    // TODO figure out how to divide this. (like how should the functions be divvied up?)

    if (res == CURLE_OK) { // web page is read and stored in page_mem_chunk.memory
      // Option 32 is HTML_PARSE_NOERROR; used to ignore errors from unescaped & symbols
      htmlDocPtr doc = htmlReadMemory(page_mem_chunk.memory, page_mem_chunk.size, url, NULL, 32);

      Game game = parse_game_page(doc);

      xmlFreeDoc(doc);
      xmlCleanupParser();
    }

    curl_easy_cleanup(curl_handle);
    free(page_mem_chunk.memory);
    return res;
  } else {
    return CURLE_FAILED_INIT;
  }
}

// TODO figure out how to return multiple things
xmlNode *WebParser::find_node(xmlNode *root_node, std::function<bool(xmlNode *)> search_func) {
  if (!root_node) {
    return root_node;
  }

  xmlNode *curr_node = NULL;

  for (curr_node = root_node; curr_node; curr_node = curr_node->next) {
    if(search_func(curr_node)) {
      return curr_node;
    } else {
      xmlNode *potential_node = find_node(curr_node->children, search_func);
      if (potential_node) {
        return potential_node;
      }
    }
  }
  return NULL;
}

bool WebParser::check_node(xmlNode *node, const char *node_type, const char *prop_type, const char *class_name) {
  if (node->type == XML_ELEMENT_NODE && !xmlStrcmp(node->name, (const xmlChar *)node_type)) {
    if (strlen(prop_type) != 0) {
      xmlChar *id = xmlGetProp(node, (const xmlChar *) prop_type);
      bool is_correct_class = !xmlStrcmp(id, (const xmlChar *) class_name);
      xmlFree(id);

      return is_correct_class;
    } else {
      // if no prop_type is given, only check the node_type
      return true;
    }
  }
  return false;
}

bool WebParser::is_clue_node(xmlNode *node) {
  return check_node(node, "td", "class", "clue");
}

bool WebParser::is_category_node(xmlNode *node) {
  return check_node(node, "td", "class", "category");
}

bool WebParser::is_jeopardy_node(xmlNode *node) {
  return check_node(node, "div", "id", "jeopardy_round");
}

bool WebParser::is_double_jeopardy_node(xmlNode *node) {
  return check_node(node, "div", "id", "double_jeopardy_round");
}

bool WebParser::is_final_jeopardy_node(xmlNode *node) {
  return check_node(node, "div", "id", "final_jeopardy_round");
}

bool WebParser::is_tr_node(xmlNode *node) {
  return check_node(node, "tr", "", "");
}

void WebParser::parse_nodes(xmlNode *root_node, std::function<void (xmlNode *, void *)> parse_func, void *parse_struct) {
  xmlNode *curr_node = NULL;

  for (curr_node = root_node; curr_node; curr_node = curr_node->next) {
    parse_func(curr_node, parse_struct);
    parse_nodes(curr_node->children, parse_func, parse_struct);
  }
}

void WebParser::parse_clue_helper(xmlNode *node, void *clue_ptr) {
  Clue *cast_clue_ptr = (Clue *)clue_ptr;

  if (node->type == XML_ELEMENT_NODE && !xmlStrcmp(node->name, (const xmlChar *)"td")) {
    xmlChar *html_class = xmlGetProp(node, (const xmlChar *)"class");

    if(!xmlStrcmp(html_class, (const xmlChar *)"clue_text")) {
      xmlNode *first_node = node->children;

      // The libxml parser doesn't like links or double hyphens (for some reason), and splits up the text node
      // So if those show up we need to do additional processing
      // if first_node->next is null, that means no additional parsing necessary
      if (!first_node->next) {
        cast_clue_ptr->m_clue = (char *) first_node->content;
      }
      // If it has a next node and it's a span, that means it's the weird double hyphen behavior
      // The best info I can find is https://gitlab.gnome.org/GNOME/libxml2/-/commit/3c0d62b4193c5c1fe15a143a138f76ffa1278779
      // So manually recreate it and get the second half of the clue (in node->next->next)
      else if (!xmlStrcmp(first_node->next->name, (const xmlChar *)"span")) {
        // TODO: there's gotta be a better way of casting
        string full_clue =
            (string) (const char *)first_node->content + "--" +
            (string) (const char *)first_node->next->next->content;

        cast_clue_ptr->m_clue = full_clue;
      }
      // Handle if it starts with a link
      else if (!xmlStrcmp(first_node->name, (const xmlChar *)"a")) {
        string full_clue =
            (string) (const char *)first_node->children->content +
            (string) (const char *)first_node->next->content;

        cast_clue_ptr->m_clue = full_clue;
        cast_clue_ptr->m_link = (char *)first_node->properties->children->content; // gets the text value of the href property
      }
      // Handle if it has a link in the middle
      else if (!xmlStrcmp(first_node->next->name, (const xmlChar *)"a")) {
        xmlNode *link_node = first_node->next;

        string full_clue =
            (string) (const char *)first_node->content +
            (string) (const char *)link_node->children->content;

        // have to add this check in case the link is at the end of the clue and there is no next
        if (link_node->next) {
          full_clue += (string) (const char *) first_node->next->next->content;
        }

        cast_clue_ptr->m_clue = full_clue;
        cast_clue_ptr->m_link = (char *)link_node->properties->children->content; // gets the text value of the href property
      }

    } else if (!xmlStrcmp(html_class, (const xmlChar *)"clue_value_daily_double")) {
      // If it has this class, then it's a daily double, but I don't care how much the contestant bet
      cast_clue_ptr->m_is_daily_double = true;
    }
    xmlFree(html_class);

  } else if (node->type == XML_ELEMENT_NODE && !xmlStrcmp(node->name, (const xmlChar *)"div")) {
    xmlChar *onmouseover = xmlGetProp(node, (const xmlChar *)"onmouseover");

    string mouseover_str = (char *)onmouseover;
    string start_substr = "<em class=\"correct_response\">";
    int start_index = mouseover_str.find(start_substr);
    if (start_index == -1) {
      // if it wasn't found, then it's probably the final jeopardy node, which has a slightly different start substr
      start_substr = "<em class=\\\"correct_response\\\">";
      start_index = mouseover_str.find(start_substr);
    }
    string end_substr = "</em>";
    int start_loc = start_index + start_substr.size();
    int end_loc = mouseover_str.find(end_substr);
    string answer_str = mouseover_str.substr(start_loc, (end_loc - start_loc));

    xmlFree(onmouseover);
    cast_clue_ptr->m_answer = answer_str;
  }
}

Clue WebParser::parse_clue(xmlNode *clue_node) {
  Clue clue;
  Clue *clue_ptr = &clue;

  // Need to call on children, because the clue_node itself has other clues in node->next
  parse_nodes(clue_node->children, parse_clue_helper, clue_ptr);

  return clue;
}

void WebParser::parse_category_name_helper(xmlNode *node, void *category_ptr) {
  Category *cast_category_ptr = (Category *)category_ptr;

  if (node->type == XML_ELEMENT_NODE && !xmlStrcmp(node->name, (const xmlChar *)"td")) {
    xmlChar *html_class = xmlGetProp(node, (const xmlChar *) "class");

    if (!xmlStrcmp(html_class, (const xmlChar *) "category_name")) {
      xmlNode *text_node = node->children;
      cast_category_ptr->m_title = (char *) text_node->content;
    }

    xmlFree(html_class);
  }
}

Category WebParser::initialize_category(xmlNode *category_node, bool is_double_jeopardy, bool is_final_jeopardy) {
  Category category;
  Category *category_ptr = &category;

  // Need to call on children, because the category_node itself has other categories in node->next
  parse_nodes(category_node->children, parse_category_name_helper, category_ptr);
  category.m_clues = vector<Clue>();
  category.m_is_double_jeopardy = is_double_jeopardy;
  category.m_is_final_jeopardy = is_final_jeopardy;

  return category;
}

vector<Category> WebParser::parse_round(xmlNode *round_node, bool is_double_jeopardy) {
  vector<Category> categories;

  xmlNode *first_category_node = find_node(round_node, is_category_node);

  for (xmlNode *curr_node = first_category_node; curr_node; curr_node = curr_node->next) {
    if (curr_node->type == XML_ELEMENT_NODE) {
      categories.push_back(initialize_category(curr_node, is_double_jeopardy, false));
    }
  }

  // First tr node is the categories, it's next is the text node, and the text node's next is the first row of clues
  xmlNode *curr_row_node = find_node(round_node, is_tr_node);
  for (int i = 0; i < 5; i++) {
    curr_row_node = curr_row_node->next->next;
    xmlNode *clue_node = find_node(curr_row_node, is_clue_node);
    for (int j = 0; j < 6; j++) {
      Clue clue = parse_clue(clue_node);

      int value = 200 * (i + 1);
      if (is_double_jeopardy) {
        value *= 2;
      }
      clue.m_value = value;

      // If the clue is blank, don't add it to the list
      if (clue.m_clue.compare("")) {
        categories[j].m_clues.push_back(clue);
      }

      clue_node = clue_node->next->next;
    }
  }

  return categories;
}

Category WebParser::parse_final_jeopardy(xmlNode *round_node) {
  xmlNode *category_node = find_node(round_node, is_category_node);
  Category category = initialize_category(category_node, false, true);

  // For final jeopardy, the answer is attached to the category HTML node (not the clue), so we have to pass the entire
  // round node to the parser
  Clue clue = parse_clue(round_node);
  clue.m_is_final_jeopardy = true;
  clue.m_value = 0;

  category.m_clues.push_back(clue);

  return category;
}

Game WebParser::parse_game_page(htmlDocPtr doc) {
  xmlNode *root_element = xmlDocGetRootElement(doc);

  Game game;

  //TODO: get air date

  xmlNode *jeopardy_node = find_node(root_element, is_jeopardy_node);
  vector<Category> single_jeopardy_categories = parse_round(jeopardy_node, false);
  game.m_single_jeopardy = single_jeopardy_categories;

  xmlNode *double_jeopardy_node = find_node(root_element, is_double_jeopardy_node);
  vector<Category> double_jeopardy_categories = parse_round(double_jeopardy_node, true);
  game.m_double_jeopardy = double_jeopardy_categories;

  for (vector<Category>::iterator it = game.m_double_jeopardy.begin(); it != game.m_double_jeopardy.end(); it++) {
    std::printf("\nCategory: %s", it->m_title.c_str());
    for (vector<Clue>::iterator jt = it->m_clues.begin(); jt != it->m_clues.end(); jt++) {
      std::printf("\nClue: %s", jt->m_clue.c_str());
    }
  }

  xmlNode *final_jeopardy_node = find_node(root_element, is_final_jeopardy_node);
  Category final_jeopardy = parse_final_jeopardy(final_jeopardy_node);
  game.m_final_jeopardy = final_jeopardy;

  return game;
}
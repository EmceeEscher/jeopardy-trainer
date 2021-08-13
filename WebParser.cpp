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

CURLcode WebParser::retrieve_web_page(const char *url, Game *game_ptr) {

  CURL *curl_handle = curl_easy_init();

  if (curl_handle) {
    struct MemoryStruct page_mem_chunk;

    page_mem_chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc in the callback */
    page_mem_chunk.size = 0;    /* no data at this point */

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&page_mem_chunk);

    CURLcode res = curl_easy_perform(curl_handle);

    if (res == CURLE_OK) { // web page is read and stored in page_mem_chunk.memory
      // Option 32 is HTML_PARSE_NOERROR; used to ignore errors from unescaped & symbols
      htmlDocPtr doc = htmlReadMemory(page_mem_chunk.memory, page_mem_chunk.size, url, NULL, 32);

      parse_game_page(doc, game_ptr);

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

bool WebParser::is_title_node(xmlNode *node) {
  return check_node(node, "div", "id", "game_title");
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
      xmlNode *curr_node = node->children;
      string clue_text = "";
      vector<string> links;

      while(curr_node) {
        if (!xmlStrcmp(curr_node->name, (const xmlChar *)"text")) {
          clue_text += (char *) curr_node->content;
        }
        // The libxml parser doesn't like double hyphens (for some reason), and splits up the text node
        // The double hyphen is put into a span node, so if we see that, just manually add the double hyphen back and move on
        else if (!xmlStrcmp(curr_node->name, (const xmlChar *)"span")) {
          clue_text += "--";
        } else if (!xmlStrcmp(curr_node->name, (const xmlChar *)"a")) {
          // Find the node with text (usually child node, but sometimes deeper)
          xmlNode *text_node = curr_node->children;
          while (xmlStrcmp(text_node->name, (const xmlChar *)"text")) {
            text_node = text_node->children;
          }

          // Add the text of the link, which is in the child node, to the clue
          clue_text += (char *) text_node->content;

          // gets the text value of the href property, and adds it to the links
          links.push_back((char *)curr_node->properties->children->content);
        }

        curr_node = curr_node->next;
      }

      cast_clue_ptr->m_clue = clue_text;
      if (!links.empty()) {
        cast_clue_ptr->m_links = links;
      }
    } else if (!xmlStrcmp(html_class, (const xmlChar *)"clue_value_daily_double")) {
      // If it has this class, then it's a daily double, but I don't care how much the contestant bet
      cast_clue_ptr->m_is_daily_double = true;
    }
    xmlFree(html_class);

  } else if (node->type == XML_ELEMENT_NODE && !xmlStrcmp(node->name, (const xmlChar *)"div")) {
    xmlChar *onmouseover = xmlGetProp(node, (const xmlChar *)"onmouseover");

    string mouseover_str = (char *)onmouseover;

    // Find answer
    bool is_final_jeopardy = false;
    string answer_start_substr = "<em class=\"correct_response\">";
    string answer_end_substr = "</em>";
    string answer = parse_string_helper(mouseover_str, answer_start_substr, answer_end_substr);

    if (answer == "") {
      // if not found, it's probably final jeopardy, which has a slightly different start substr
      answer_start_substr = "<em class=\\\"correct_response\\\">";
      answer = parse_string_helper(mouseover_str, answer_start_substr, answer_end_substr);
      is_final_jeopardy = true;
    }

    // Find comments (if there are any)
    string comment_start_substr = "stuck', '(";
    string comment_end_substr = is_final_jeopardy ? ")<table>" : "<br /><br /><em";
    string comment = parse_string_helper(mouseover_str, comment_start_substr, comment_end_substr);

    xmlFree(onmouseover);
    cast_clue_ptr->m_answer = answer;
    if (comment != "") {
      cast_clue_ptr->m_comments = "(" + comment;
    }
  }
}

string WebParser::parse_string_helper(std::string full_str, std::string start_substr, std::string end_substr) {
  int start_index = full_str.find(start_substr);
  if (start_index == -1) {
    return "";
  }
  int start_loc = start_index + start_substr.size();
  int end_loc = full_str.find(end_substr);
  return full_str.substr(start_loc, (end_loc - start_loc));
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
      // TODO: figure out the category type here? based on the title?

      xmlNode *text_node = node->children;
      if (!xmlStrcmp(text_node->name, (const xmlChar *) "text")) {
        cast_category_ptr->m_title = (char *) text_node->content;
      } else if (!xmlStrcmp(text_node->name, (const xmlChar *) "a")) {

        // Normally, the text of the link is in the child node, but occasionally it can be split up due to extra
        // formatting nodes
        xmlNode *title_node = text_node->children;
        string title_builder = "";

        while (title_node) {
          if (!xmlStrcmp(title_node->name, (const xmlChar *) "text")) {
            title_builder += (char *) title_node->content;
          } else {
            title_builder += (char *) title_node->children->content;
          }

          title_node = title_node->next;
        }

        cast_category_ptr->m_title = title_builder;

        // gets the text value of the href property, and adds it to the links
        cast_category_ptr->m_link = (char *) text_node->properties->children->content;
      }
    }

    if (!xmlStrcmp(html_class, (const xmlChar *) "category_comments")) {
      xmlNode *text_node = node->children;
      if (text_node) {
        cast_category_ptr->m_comments = (char *) text_node->content;
      }
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

void WebParser::parse_game_page(htmlDocPtr doc, Game *game_ptr) {
  xmlNode *root_element = xmlDocGetRootElement(doc);

  xmlNode *title_node = find_node(root_element, is_title_node);
  // title hierarchy is usually div->h1->text
  // however, sometimes there are italics in the title, so we need to go for the next node in that case
  xmlNode *day_node = title_node->children->children;
  if (!xmlStrcmp(day_node->name, (const xmlChar *)"i")) {
    day_node = day_node->next;
  }
  string full_title = (char *)day_node->content;
  // just extract the date from string of form "Show #1234 - Monday, January 1, 1000"
  game_ptr->m_air_date = full_title.substr(full_title.find("day, ") + 5);

  xmlNode *jeopardy_node = find_node(root_element, is_jeopardy_node);
  vector<Category> single_jeopardy_categories = parse_round(jeopardy_node, false);
  game_ptr->m_single_jeopardy = single_jeopardy_categories;

  xmlNode *double_jeopardy_node = find_node(root_element, is_double_jeopardy_node);
  vector<Category> double_jeopardy_categories = parse_round(double_jeopardy_node, true);
  game_ptr->m_double_jeopardy = double_jeopardy_categories;

  xmlNode *final_jeopardy_node = find_node(root_element, is_final_jeopardy_node);
  Category final_jeopardy = parse_final_jeopardy(final_jeopardy_node);
  game_ptr->m_final_jeopardy = final_jeopardy;
}

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
  struct MemoryStruct page_mem_chunk;

  page_mem_chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc in the callback */
  page_mem_chunk.size = 0;    /* no data at this point */

  CURL *curl_handle = curl_easy_init();

  if (curl_handle) {
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&page_mem_chunk);

    CURLcode res = curl_easy_perform(curl_handle);

    // TODO figure out how to divide this. (like how should the functions be divvied up?)

    if (res == CURLE_OK) { // web page is read and stored in page_mem_chunk.memory
      htmlDocPtr doc = htmlReadMemory(page_mem_chunk.memory, page_mem_chunk.size, url, NULL, 0);
      xmlNode *root_element = xmlDocGetRootElement(doc);

      // TODO find all clues, and also build categories
      xmlNode *jeopardy_node = find_node(root_element, is_jeopardy_node);
      vector<Category> categories = parse_round(jeopardy_node);

      xmlFreeDoc(doc);
      xmlCleanupParser();
    }

    curl_easy_cleanup(curl_handle);
    free(page_mem_chunk.memory);
    return res;
  } else {
    free(page_mem_chunk.memory);
    return CURLE_FAILED_INIT;
  }

  //free(page_mem_chunk.memory);
  // TODO figure out what to return here (probably not a CURLcode)
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
      xmlNode *text_node = node->children;
      cast_clue_ptr->m_clue = (char *)text_node->content;

    } else if (!xmlStrcmp(html_class, (const xmlChar *)"clue_value")) {
      xmlNode *text_node = node->children;
      // Strings are of form "$200". Strip the $ and then convert to int
      string money_string = (char *)text_node->content;
      cast_clue_ptr->m_value = stoi(money_string.substr(1));

      // TODO test daily double case
    } else if (!xmlStrcmp(html_class, (const xmlChar *)"clue_value_daily_double")) {
      xmlNode *text_node = node->children;
      // Strings are of form "DD: $200". Strip the prefix and then convert to int
      string money_string = (char *)text_node->content;
      cast_clue_ptr->m_value = stoi(money_string.substr(5));
      cast_clue_ptr->m_is_daily_double = true;

    }
    xmlFree(html_class);

  } else if (node->type == XML_ELEMENT_NODE && !xmlStrcmp(node->name, (const xmlChar *)"div")) {
    xmlChar *onmouseover = xmlGetProp(node, (const xmlChar *)"onmouseover");

    string mouseover_str = (char *)onmouseover;
    string start_substr = "<em class=\"correct_response\">";
    string end_substr = "</em>";
    int start_loc = mouseover_str.find(start_substr) + start_substr.size();
    int end_loc = mouseover_str.find(end_substr);
    string answer_str = mouseover_str.substr(start_loc, (end_loc - start_loc));

    cast_clue_ptr->m_answer = answer_str;

    xmlFree(onmouseover);
  }
}

// TODO will need to write a separate Final Jeopardy parser bc that's different
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

Category WebParser::initialize_category(xmlNode *category_node) {
  Category category;
  Category *category_ptr = &category;

  // Need to call on children, because the category_node itself has other categories in node->next
  parse_nodes(category_node->children, parse_category_name_helper, category_ptr);
  category.m_clues = vector<Clue>();

  return category;
}

vector<Category> WebParser::parse_round(xmlNode *round_node) {
  vector<Category> categories;

  xmlNode *first_category_node = find_node(round_node, is_category_node);

  for (xmlNode *curr_node = first_category_node; curr_node; curr_node = curr_node->next) {
    if (curr_node->type == XML_ELEMENT_NODE) {
      categories.push_back(initialize_category(curr_node));
    }
  }

  // First tr node is the categories, it's next is the text node, and the text node's next is the first row of clues
  xmlNode *first_clue_row_node = find_node(round_node, is_tr_node)->next->next;
  //TODO: verify that this is actually the first clue row, and parse them, and add them to the category

  return categories;
}


//  Game parse_game_page(CURLcode page_data) {
//    htmlDocPtr doc = htmlReadMemory(page_data, (unsigned)strlen(page_data), "noname.html", NULL, 0);
//  }
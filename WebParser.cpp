#include "WebParser.h"

#include <iostream>
#include <libxml/HTMLparser.h>
#include <libxml/xmlsave.h>

using std::string;
using game::Game;
using namespace web_parser;

// This function is taken almost verbatim from a libcurl tutorial: https://curl.se/libcurl/c/getinmemory.html
size_t WebParser::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userdata) {
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
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&page_mem_chunk);

    CURLcode res = curl_easy_perform(curl_handle);

    // TODO figure out what to do with page_mem_chunk (call HTML parser here?)

    std::cout << res << std::endl;

    curl_easy_cleanup(curl_handle);
  } else {
  }

  free(page_mem_chunk.memory);
  // TODO figure out what to return here (probably not a CURLcode)
}

//  Game parse_game_page(CURLcode page_data) {
//    htmlDocPtr doc = htmlReadMemory(page_data, (unsigned)strlen(page_data), "noname.html", NULL, 0);
//  }
#include "WebParser.h"

#include <iostream>

using std::string;

namespace web_parser {
  CURLcode retrieve_web_page(const char *url) {
    CURL *curl_handle = curl_easy_init();

    if (curl_handle) {
      curl_easy_setopt(curl_handle, CURLOPT_URL, url);
      CURLcode res = curl_easy_perform(curl_handle);

      std::cout << res << std::endl;

      curl_easy_cleanup(curl_handle);
      return res;
    } else {
      return CURLE_FAILED_INIT;
    }
  }
}
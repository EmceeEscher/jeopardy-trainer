cmake_minimum_required(VERSION 3.17)
project(jeopardy_trainer)

set(CMAKE_CXX_STANDARD 14)

# TODO: find a better way than hardcoding the directory
set(ZLIB_INCLUDE_DIR "/Users/jacob/projects/libraries/vcpkg/installed/x64-osx/include")
set(ZLIB_LIBRARY 1)
find_package(ZLIB REQUIRED)
set(CURL_DIR "/Users/jacob/projects/libraries/vcpkg/installed/x64-osx/share/curl")
find_package(CURL CONFIG REQUIRED)

add_executable(jeopardy_trainer main.cpp Clue.h Clue.cpp Category.cpp Category.h Game.cpp Game.h)
target_link_libraries(jeopardy_trainer PRIVATE ZLIB::ZLIB)
target_link_libraries(jeopardy_trainer PRIVATE CURL::libcurl)
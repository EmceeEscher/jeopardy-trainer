cmake_minimum_required(VERSION 3.17)
project(jeopardy_trainer)

set(CMAKE_CXX_STANDARD 14)

# TODO: find a better way than hardcoding the directory. Can I copy what the libxml thing is doing?
set(ZLIB_INCLUDE_DIR "/Users/jacob/projects/libraries/vcpkg/installed/x64-osx/include")
find_package(ZLIB REQUIRED)
set(CURL_DIR "/Users/jacob/projects/libraries/vcpkg/installed/x64-osx/share/curl")
find_package(CURL CONFIG REQUIRED)
find_package(LibXml2 REQUIRED)

find_package(PostgreSQL)
set(libpqxx_DIR "/Users/jacob/projects/libraries/vcpkg/installed/x64-osx/share/libpqxx")
find_package(libpqxx CONFIG REQUIRED)

add_executable(jeopardy_trainer main.cpp Clue.h Clue.cpp Category.cpp Category.h Game.cpp Game.h WebParser.cpp WebParser.h)
target_link_libraries(jeopardy_trainer PRIVATE ZLIB::ZLIB)
target_link_libraries(jeopardy_trainer PRIVATE CURL::libcurl)
target_include_directories(jeopardy_trainer PRIVATE ${LIBXML2_INCLUDE_DIR})
target_link_libraries(jeopardy_trainer PRIVATE ${LIBXML2_LIBRARIES})
target_link_libraries(jeopardy_trainer PRIVATE libpqxx::pqxx)
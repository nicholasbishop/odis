cmake_minimum_required(VERSION 2.8)

set(SRC file_info_elf.cc main.cc sqlite.cc)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wextra -std=c++0x")

FIND_PATH(SQLITE3_INCLUDE_DIR sqlite3.h)

include_directories(test ${SQLITE3_INCLUDE_DIR})

add_executable(test ${SRC})

target_link_libraries(test elf sqlite3)
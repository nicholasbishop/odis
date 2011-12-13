cmake_minimum_required(VERSION 2.8)

set(SRC app.cc file_info_elf.cc main.cc project.cc sqlite.cc)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wextra -std=c++0x")

FIND_PATH(SQLITE3_INCLUDE_DIR sqlite3.h)

# GTK+
find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)

include_directories(test ${GTKMM_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIR})

add_executable(test ${SRC})

target_link_libraries(test elf sqlite3 ${GTKMM_LIBRARIES})
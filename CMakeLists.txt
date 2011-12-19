# This file is part of Odis.
#
# Odis is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Odis is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Odis.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.8)

set(SRC app.cc file_info_elf.cc hex.cc main.cc project.cc sqlite.cc)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wextra -std=c++0x")

FIND_PATH(SQLITE3_INCLUDE_DIR sqlite3.h)

# GTK+
find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)

include_directories(test ${GTKMM_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIR})

add_executable(test ${SRC})

target_link_libraries(test elf sqlite3 ${GTKMM_LIBRARIES})

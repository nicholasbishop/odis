/* This file is part of Odis.

   Odis is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Odis is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Odis.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef UTIL_HH
#define UTIL_HH

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace Odis {
	template<typename T> std::string to_string(const T& t) {
		std::stringstream ss;
		ss << t;
		return ss.str();
	}

	template<typename T> std::string hex(const T& t) {
		std::stringstream ss;
		ss << "0x" << std::hex << t;
		return ss.str();
	}
}

#endif

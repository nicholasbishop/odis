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

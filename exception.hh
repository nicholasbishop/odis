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

#ifndef EXCEPTION_HH
#define EXCEPTION_HH

#include <exception>
#include <string>

namespace Odis {
	class Exception : public std::exception {
	public:
		Exception(const char *description_) : description(description_) {}

		virtual const char* what() const throw() {
			return description.c_str();
		};

		virtual ~Exception() throw() {};
		
	private:
		const std::string description;
	};
}

#endif

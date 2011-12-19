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

#ifndef PROJECT_HH
#define PROJECT_HH

#include "sqlite.hh"
#include <string>

namespace Odis {
	class Project {
	public:
		Project(const std::string& filename);

		Sqlite& database();
		std::string name();

		void add_file(const std::string& filepath);
		
	private:
		Project();

		void init_db();
		
		Sqlite db;
	};
}

#endif

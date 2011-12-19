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

#include "app.hh"

using namespace Odis;

#if 0
int main(int argc, char **argv) {
	if(argc != 2)
		throw std::exception();

	Sqlite db("test.sqlite3");
	init_project_db(db);
	/* load the input file into the project database */
	project_db_add_file(db, argv[1]);

	/* tmp */
	size_t file_length;
	char *data = read_file(argv[1], &file_length);
	FileInfo *info = file_info_elf(data, file_length);
	Sqlite::Stmt stmt(db, "INSERT INTO part VALUES(?,?,?,?,?,?,?)");
	
	for(auto parts : info->parts()) {
		for(auto p : parts.second) {
			stmt.bind_text_with_copy(1, parts.first.c_str());
			stmt.bind_int(2, 1); /* TODO */
			stmt.bind_text_with_copy(3, p.name.c_str());
			stmt.bind_int(4, p.file_offset);
			stmt.bind_int(5, p.file_size);
			stmt.bind_int(6, p.virtual_addr);
			stmt.bind_int(7, p.virtual_size);
			db.step(stmt);
			stmt.reset();
		}
	}
	delete info;
	delete data;
}
#else

int main(int argc, char *argv[]) {
	App(argc, argv);

	return 0;
}

#endif

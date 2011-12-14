#include "file_info.hh"
#include "project.hh"
#include <gtkmm.h>

using namespace Odis;

Project::Project(const std::string& filepath) : db(filepath) {
	/* TODO */
	init_db();
}

Sqlite& Project::database() {
	return db;
}

std::string Project::name() {
	std::string db_name = db.name();
	if(db.name() == ":memory:")
		return "Untitled";
	else
		return db_name;
}

void Project::add_file(const std::string& filepath) {
	std::string file_data(Glib::file_get_contents(filepath));
	
	Sqlite::Stmt stmt(db, "INSERT INTO file VALUES(?, ?)");
	stmt.bind_text_static(1, filepath.c_str());
	stmt.bind_blob_static(2, file_data.data(), file_data.size());
	db.step(stmt);

	/* TODO */
	/* check if ELF */
	if(file_data[0] == 0x7f &&
	   file_data[1] == 'E' &&
	   file_data[2] == 'L' &&
	   file_data[3] == 'F') {
		FileInfo *info = file_info_elf(file_data.data(), file_data.size());
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
	}
}

void Project::init_db() {
	/* create file table */
	db.exec("CREATE TABLE IF NOT EXISTS file "
			"(filepath TEXT, data BLOB)");

	/* create part table */
	db.exec("CREATE TABLE IF NOT EXISTS part "
			"(part_group TEXT, file_id INTEGER, "
			"name TEXT, "
			"file_offset INTEGER, file_size INTEGER, "
			"virtual_addr INTEGER, virtual_size INTEGER)");
}

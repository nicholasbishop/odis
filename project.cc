#include "project.hh"
#include <gtkmm.h>

using namespace Odis;

Project::Project(const char *filepath) : db(filepath) {
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
	std::string *file_data = new std::string(Glib::file_get_contents(filepath));
	
	Sqlite::Stmt stmt(db, "INSERT INTO file VALUES(?, ?)");
	stmt.bind_text_static(1, filepath.c_str());
	stmt.bind_blob_with_free(2, file_data, file_data->size(), operator delete[]);
	db.step(stmt);
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

#include "sqlite.hh"
#include "util.hh"
#include <cstring>
#include <sqlite3.h>
#include <string>

using namespace Odis;

Sqlite::Sqlite(const std::string& db_filename_)
	throw(SqliteException) : db(0), db_filename(db_filename_) {
    int rc;
	
    if((rc = sqlite3_open(db_filename.c_str(), &db))) {
		sqlite3_close(db);
		throw SqliteException("sqlite3_open failed");
	}
}

Sqlite::~Sqlite() {
	if(db)
		sqlite3_close(db);
}

const std::string& Sqlite::name() {
	return db_filename;
}

void Sqlite::exec(const char *arg) throw(SqliteException) {
	int rc;
	
	rc = sqlite3_exec(db, arg, 0, 0, 0);
	check_rc(rc);
}

Sqlite::StepResult Sqlite::step(Stmt &stmt) throw(SqliteException) {
	int rc = sqlite3_step(stmt.stmt);

	switch(rc) {
	case SQLITE_DONE:
		return DONE;
	case SQLITE_ROW:
		return ROW;
	default:
		check_rc(rc);
	}

	throw SqliteException("unknown step error");
}

void Sqlite::check_rc(int rc) throw(SqliteException)
{
	switch(rc) {
	case SQLITE_OK:
	case SQLITE_DONE:
		return;
	default:
		throw SqliteException((std::string(sqlite3_errmsg(db)) +
							   ", code " +
							   to_string(rc)).c_str());
	}
}

Sqlite::Stmt::Stmt(Sqlite &sqlite_, const std::string& text) throw(SqliteException)
	: stmt(0), sqlite(sqlite_) {
	int rc;

	rc = sqlite3_prepare_v2(sqlite.db, 
							text.c_str(),
							text.size(),
							&stmt,
							0);
	sqlite.check_rc(rc);
}

Sqlite::Stmt::~Stmt() {
	if(stmt)
		sqlite3_finalize(stmt);
}

void Sqlite::Stmt::reset() {
	sqlite3_reset(stmt);
}

void Sqlite::Stmt::bind_int(int index_from_1,
							int64_t val)
	throw(SqliteException) {
	int rc;

	rc = sqlite3_bind_int64(stmt,
						  index_from_1,
						  val);

	sqlite.check_rc(rc);
}


void Sqlite::Stmt::bind_text_static(int index_from_1,
									const char *str)
	throw(SqliteException) {
	int rc;

	rc = sqlite3_bind_text(stmt,
						   index_from_1,
						   str,
						   strlen(str) + 1,
						   SQLITE_STATIC);
	sqlite.check_rc(rc);
}

void Sqlite::Stmt::bind_text_with_copy(int index_from_1,
									   const char *str)
	throw(SqliteException) {
	int rc;

	rc = sqlite3_bind_text(stmt,
						   index_from_1,
						   str,
						   strlen(str) + 1,
						   SQLITE_TRANSIENT);
	sqlite.check_rc(rc);
}

void Sqlite::Stmt::bind_blob_with_copy(int index_from_1,
									   const void *blob_data,
									   int blob_data_size)
	throw(SqliteException) {
	int rc;

	rc = sqlite3_bind_blob(stmt,
						   index_from_1,
						   blob_data,
						   blob_data_size,
						   SQLITE_TRANSIENT);
	sqlite.check_rc(rc);
}

void Sqlite::Stmt::bind_blob_with_free(int index_from_1,
									   const void *blob_data,
									   int blob_data_size,
									   void (*free)(void*))
	throw(SqliteException) {
	int rc;

	rc = sqlite3_bind_blob(stmt,
						   index_from_1,
						   blob_data,
						   blob_data_size,
						   free);
	sqlite.check_rc(rc);
}

std::string Sqlite::Stmt::column_text(int n) {
	return std::string((const char*)sqlite3_column_text(stmt, n));
}

int64_t Sqlite::Stmt::column_int(int n) {
	return sqlite3_column_int64(stmt, n);
}

#include "sqlite.hh"
#include <cstring>
#include <sqlite3.h>
#include <string>
#include <sstream>

using namespace Odis;

Sqlite::Sqlite(const char *db_filename) throw(SqliteException) : db(0) {
    int rc;
	
    if((rc = sqlite3_open(db_filename, &db))) {
		sqlite3_close(db);
		throw SqliteException("sqlite3_open failed");
	}
}

Sqlite::~Sqlite() {
	if(db)
		sqlite3_close(db);
}

void Sqlite::exec(const char *arg) throw(SqliteException) {
	char *zErrMsg = 0;
	int rc;
	
	rc = sqlite3_exec(db, arg, 0, 0, &zErrMsg);
	check_rc(rc, zErrMsg);
}

Sqlite::StepResult Sqlite::step(Stmt &stmt) throw(SqliteException) {
	int rc = sqlite3_step(stmt.stmt);

	switch(rc) {
	case SQLITE_DONE:
		return DONE;
	default:
		check_rc(rc);
	}

	throw SqliteException("unknown step error");
}

void Sqlite::check_rc(int rc, char *err) throw(SqliteException)
{
	std::stringstream ss;

	/* if an error is passed in, free it */
	if(err) {
		ss << err << ": " << rc;
		sqlite3_free(err);
	}
	else
		ss << "error code " << rc;

	switch(rc) {
	case SQLITE_OK:
	case SQLITE_DONE:
		return;
	default:
		throw SqliteException(ss.str().c_str());
	}
}

Sqlite::Stmt::Stmt(Sqlite &sqlite_, const char *text) throw(SqliteException)
	: stmt(0), sqlite(sqlite_) {
	int rc;

	rc = sqlite3_prepare_v2(sqlite.db, 
							text,
							strlen(text) + 1, 
							&stmt,
							0);
	sqlite.check_rc(rc);
}

Sqlite::Stmt::~Stmt() {
	if(stmt)
		sqlite3_finalize(stmt);
}

void Sqlite::Stmt::bind_text_static(int index_from_1,
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

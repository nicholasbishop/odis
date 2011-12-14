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

void Sqlite::save(const std::string& path) throw(SqliteException) {
	/* TODO */
	if(db_filename != ":memory:")
		throw SqliteException("saving but db not in mem");

	int rc;                   /* Function return code */
	sqlite3 *db_file;           /* Database connection opened on zFilename */
	sqlite3_backup *backup;  /* Backup object used to copy data */

	/* Open the database file identified by zFilename. Exit early if this fails
	** for any reason. */
	rc = sqlite3_open(path.c_str(), &db_file);
	check_rc(rc);

	/* Set up the backup procedure to copy from the "main" database of 
	** connection pFile to the main database of connection pInMemory.
	** If something goes wrong, pBackup will be set to NULL and an error
	** code and  message left in connection pTo.
	**
	** If the backup object is successfully created, call backup_step()
	** to copy data from pFile to pInMemory. Then call backup_finish()
	** to release resources associated with the pBackup object.  If an
	** error occurred, then  an error code and message will be left in
	** connection pTo. If no error occurred, then the error code belonging
	** to pTo is set to SQLITE_OK.
	*/
	backup = sqlite3_backup_init(db_file, "main", db, "main");
	if(backup) {
		sqlite3_backup_step(backup, -1);
		sqlite3_backup_finish(backup);
	}
	rc = sqlite3_errcode(db_file);
	check_rc(rc);

	/* close the in-memory db and replace the local handle with the
	   on-disk db */
	sqlite3_close(db);
	db = db_file;
	db_filename = path;
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

void Sqlite::Stmt::bind_blob_static(int index_from_1,
									const void *blob_data,
									int blob_data_size)
	throw(SqliteException) {
	int rc;

	rc = sqlite3_bind_blob(stmt,
						   index_from_1,
						   blob_data,
						   blob_data_size,
						   SQLITE_STATIC);
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

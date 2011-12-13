#ifndef SQLITE_HH
#define SQLITE_HH

#include "exception.hh"

struct sqlite3;
struct sqlite3_stmt;

namespace Odis {
	class Sqlite {
	public:
		class SqliteException : public Exception {
		public:
			SqliteException(const char *description)
				: Exception(description) {}
		};

		Sqlite(const char *db_filename) throw(SqliteException);
		~Sqlite();

		void exec(const char *arg) throw(SqliteException);
		
		enum StepResult {
			DONE,
		};

		class Stmt {
		public:
			Stmt(Sqlite &sqlite_, const char *text) throw(SqliteException);
			~Stmt();

			void bind_text_static(int index_from_1,
								  const char *str) throw(SqliteException);
			void bind_blob_with_copy(int index_from_1,
								const void *blob_data,
								int blob_data_size) throw(SqliteException);
			void bind_blob_with_free(int index_from_1,
									 const void *blob_data,
									 int blob_data_size,
									 void (*free)(void*))
				throw(SqliteException);

			struct sqlite3_stmt *stmt;
		private:
			Sqlite &sqlite;
		};

		StepResult step(Stmt &stmt) throw(SqliteException);

	private:
		/* if err is non-null, it will be free'd with sqlite3_free() */
		void check_rc(int rc, char *err = 0) throw(SqliteException);

		struct sqlite3 *db;
	};
}

#endif
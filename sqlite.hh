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

#ifndef SQLITE_HH
#define SQLITE_HH

#include "exception.hh"
#include <string>
#include <tuple>
#include <vector>

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

		Sqlite(const std::string& db_filename_) throw(SqliteException);
		~Sqlite();

		void save(const std::string& path) throw(SqliteException);

		const std::string& name();

		void exec(const char *arg) throw(SqliteException);

		enum StepResult {
			DONE,
			ROW,
		};

		class Stmt {
		public:
			Stmt(Sqlite &sqlite_, const std::string& text) throw(SqliteException);
			~Stmt();

			void reset();
			
			void bind_int(int index_from_1,
						  int64_t val) throw(SqliteException);
			void bind_text_static(int index_from_1,
								  const char *str) throw(SqliteException);
			void bind_text_with_copy(int index_from_1,
									 const char *str) throw(SqliteException);
			void bind_blob_static(int index_from_1,
								  const void *blob_data,
								  int blob_data_size) throw(SqliteException);
			void bind_blob_with_copy(int index_from_1,
								const void *blob_data,
								int blob_data_size) throw(SqliteException);
			void bind_blob_with_free(int index_from_1,
									 const void *blob_data,
									 int blob_data_size,
									 void (*free)(void*))
				throw(SqliteException);

			std::string column_text(int n);
			int64_t column_int(int n);
			const void* column_blob(int n, int* size);

			struct sqlite3_stmt *stmt;
		private:
			Sqlite &sqlite;
		};

		StepResult step(Stmt &stmt) throw(SqliteException);

	private:
		void check_rc(int rc) throw(SqliteException);

		struct sqlite3 *db;
		std::string db_filename;
	};
}

#endif

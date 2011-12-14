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

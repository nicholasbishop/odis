#include "file_info.hh"
#include "sqlite.hh"
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

using namespace Odis;

void init_project_db(Sqlite &db) {
	db.exec("CREATE TABLE IF NOT EXISTS file "
			"(filepath TEXT, data BLOB)");
}

char *read_file(const char *filepath, size_t *file_length) {
	/* open file */
	int fd = open(filepath, O_RDONLY);

	/* get file size */
	struct stat s;
	if(fstat(fd, &s))
		throw Exception("fstat failed");
	*file_length = s.st_size;

	/* read file data */
	char *data = new char[s.st_size];
	ssize_t rc;
	size_t bytes_read = 0;
	while((rc = read(fd, data + bytes_read, s.st_size - bytes_read)) > 0)
		bytes_read += rc;
	if(rc)
		throw Exception("read failed");
	if(bytes_read != (size_t)s.st_size)
		throw Exception("wrong number of bytes read");

	close(fd);
	return data;
}

void project_db_add_file(Sqlite &db, const char *filepath) {
	size_t file_length;
	char *file_data = read_file(filepath, &file_length);
	
	Sqlite::Stmt stmt(db, "INSERT INTO file VALUES(?, ?)");
	stmt.bind_text_static(1, filepath);
	stmt.bind_blob_with_free(2, file_data, file_length, operator delete[]);
	db.step(stmt);
}

int main(int argc, char **argv) {
	/*try {
	  Loader *loader = Odis::elf_loader_create_from_file(argv[1]);
	  LoaderOutput *output = new LoaderOutputSQL();
		
	  while(loader->read(output));
	  delete loader;
	  }
	  catch(std::exception *e) {
	  std::cerr << e->what() << std::endl;
	  }*/

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
	for(auto parts : info->parts()) {
		std::cout << parts.first << ": " << std::endl;

		for(auto p : parts.second) {
			std::cout << "  " << p.name << std::hex
					  << ", file(0x" << p.file_offset << ", 0x" << p.file_size
					  << "), virt(0x" << p.virtual_addr << ", 0x" << p.virtual_size
					  << ")" << std::endl;
		}
	}
	delete info;
	delete data;
}

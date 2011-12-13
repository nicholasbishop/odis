#include "file_info.hh"
#include "util.hh"

#include <fcntl.h>
#include <gelf.h>
#include <iostream>
#include <unistd.h>

namespace Odis {
	class FileInfoElf : public FileInfo {
	public:
		class FileInfoElfException : public FileInfoException {
		public:
			FileInfoElfException(const char *description)
				: FileInfoException(description) {}
		};

		FileInfoElf(const void *mem, uint64_t mem_size)
			throw(FileInfoElfException)
			: elf(0) {

			init_libelf();
			load_elf(mem, mem_size);

			/* load segments */
			std::vector<Part> &seg = part_map["segments"];
			size_t max_phdr;
			if(elf_getphdrnum(elf, &max_phdr))
				throw FileInfoElfException("elf_getphdrnum() failed");
			for(size_t i = 0; i < max_phdr; i++) {
				GElf_Phdr phdr;
				if(gelf_getphdr(elf, i, &phdr) != &phdr)
					throw FileInfoElfException("gelf_getphdr() failed");
				
				seg.push_back(Part(phdr.p_offset,
								   phdr.p_filesz,
								   phdr.p_vaddr,
								   phdr.p_memsz,
								   segment_name(phdr)));
			}

			/* load sections */
			//std::vector<Part> &sec = part_map["sections"];
		}

		~FileInfoElf() {
			if(elf)
				elf_end(elf);
		}

	private:
		/* initialize libelf */
		void init_libelf() throw(FileInfoElfException) {
			if(elf_version(EV_CURRENT) == EV_NONE)
				throw FileInfoElfException("elf_version() is EV_NONE");
		}

		/* load elf data */
		void load_elf(const void *mem, uint64_t mem_size)
			throw(FileInfoElfException) {
			if(!(elf = elf_memory((char*)mem, mem_size)))
				throw FileInfoElfException("elf_memory() failed");

			/* check type */
			if(elf_kind(elf) != ELF_K_ELF)
				throw FileInfoElfException("elf_kind() is not ELF_K_ELF");
		}

		/* convert program header type to a string */
		std::string segment_name(GElf_Phdr &phdr) {
			std::string name;
			switch(phdr.p_type & 7) {
			case PT_NULL:
				return "NULL";
			case PT_LOAD:
				return "LOAD";
			case PT_DYNAMIC:
				return "DYNAMIC";
			case PT_INTERP:
				return "INTERP";
			case PT_NOTE:
				return "NOTE";
			case PT_SHLIB:
				return "SHLIB";
			case PT_PHDR:
				return "PHDR";
			default:
				return hex(phdr.p_type);
			}
		}
	
		Elf *elf;
	};

	FileInfo *file_info_elf(const void *mem, uint64_t mem_size)
		throw(FileInfo::FileInfoException) {
		return new FileInfoElf(mem, mem_size);
	}
	
}

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
			load_segments();
			load_sections();
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
		std::string segment_type(GElf_Phdr &phdr) const {
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

		void load_segments() {
			std::vector<Part>& seg = part_map["segments"];
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
								   segment_type(phdr)));
			}
		}

		void load_sections() {
			std::vector<Part>& parts = part_map["sections"];

			size_t shstrndx;
			if(elf_getshdrstrndx(elf, &shstrndx))
				throw FileInfoElfException("elf_getshdrstrndx() failed");

			Elf_Scn* scn = nullptr;
			while((scn = elf_nextscn(elf, scn))) {
				GElf_Shdr shdr;

				if(gelf_getshdr(scn, &shdr) != &shdr)
					throw FileInfoElfException("gelf_getshdr() failed");

				char *name;
				if(!(name = elf_strptr(elf, shstrndx, shdr.sh_name)))
					throw FileInfoElfException("elf_strptr() failed");
				
				parts.push_back(Part(shdr.sh_offset,
									 shdr.sh_size,
									 shdr.sh_addr,
									 shdr.sh_size,
									 name));
			}
		}
	
		Elf *elf;
	};

	FileInfo *file_info_elf(const void *mem, uint64_t mem_size)
		throw(FileInfo::FileInfoException) {
		return new FileInfoElf(mem, mem_size);
	}
	
}

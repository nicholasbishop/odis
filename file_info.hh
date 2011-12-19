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

#include "exception.hh"
#include <cstdint>
#include <map>
#include <vector>

namespace Odis {
	/* various formats contain information on parts of the file,
	   e.g. segments or sections; there may be more than one type of
	   part for a particular file as well */
	struct Part {
		uint64_t file_offset;
		uint64_t file_size;
			
		uint64_t virtual_addr;
		uint64_t virtual_size;

		std::string name;

		Part(uint64_t file_offset_,
			 uint64_t file_size_,
			 uint64_t virtual_addr_,
			 uint64_t virtual_size_,
			 std::string name_)
			: file_offset(file_offset_),
			  file_size(file_size_),
			  virtual_addr(virtual_addr_),
			  virtual_size(virtual_size_),
			  name(name_) {
		}
	};

	typedef std::map<std::string, std::vector<Part>> PartMap;
	
	/* interface for getting info from executables, shared libs, etc. */
	class FileInfo {
	public:
		virtual ~FileInfo() {};

		virtual const PartMap& parts() {
			return part_map;
		};

		class FileInfoException : public Exception {
		public:
			FileInfoException(const char *description)
				: Exception(description) {}
		};

	protected:
		PartMap part_map;
	};

	/* where this goes is TODO */
	FileInfo *file_info_elf(const void *mem, uint64_t mem_size)
		throw(FileInfo::FileInfoException);
}

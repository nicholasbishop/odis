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

#include "hex.hh"
#include "util.hh"
#include <iomanip>
#include <iostream>

using namespace Odis;

HexView::HexView() {
	auto font_desc = Pango::FontDescription("Monospace");
	override_font(font_desc);
	set_editable(false);
}

void HexView::set_data(const void *data, uint64_t size) {
	Glib::ustring text;
	std::stringstream ss;

	ss << std::hex << std::setfill('0');

	for(uint64_t i = 0; i < size; i += 16) {
		/* clear */
		ss.str(std::string());

		/* address */
		ss << std::setw(8) << i << " ";

		/* bytes */
		for(uint64_t j = 0; i+j < size && j < 16; j++) {
			if(!(j%8))
				ss << " ";
			ss << std::setw(2) << (int)(((unsigned char*)data)[i+j]) << " ";
		}
		
		text += ss.str() + " \n";
	}
	
	get_buffer()->set_text(text);
}

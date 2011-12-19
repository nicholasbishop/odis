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

#ifndef HEX_HH
#define HEX_HH

#include <gtkmm.h>

namespace Odis {
	class HexView : public Gtk::TextView {
	public:
		HexView();

		void set_data(const void *data, uint64_t size);
	};
}

#endif

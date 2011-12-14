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

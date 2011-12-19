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

#ifndef APP_HH
#define APP_HH

#include "project.hh"
#include <gtkmm.h>

namespace Odis {
	class ProjectTreeView;
	class HexView;

	class App {
	public:
		App(int argc, char **argv);

	private:
		void init_ui_manager();
		
		/* top-level gtk stuff */
		Gtk::Main kit;
		Gtk::Window window;
		Glib::RefPtr<Gtk::UIManager> ui_manager;
		ProjectTreeView *project_tree_view;
		HexView *hex_view;

		/* handlers */
		void new_project();
		void open_project();
		void save_project();
		void add_file();
		
		/* project */
		void set_project(Project* project);
		Project* project;

		void update_hex_view();
	};
}

#endif

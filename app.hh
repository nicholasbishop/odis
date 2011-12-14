#ifndef APP_HH
#define APP_HH

#include "project.hh"
#include <gtkmm.h>

namespace Odis {
	class ProjectTreeView;

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

		/* handlers */
		void new_project();
		void save_project();
		void add_file();
		
		/* project */
		void set_project(Project* project);
		Project* project;
	};
}

#endif

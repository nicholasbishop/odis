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

#include "app.hh"
#include "hex.hh"
#include "util.hh"
#include <iomanip>
#include <iostream>

using namespace Odis;
using namespace sigc;

namespace Odis {
	class ProjectTreeView : public Gtk::TreeView {
	private:
		class ModelColumns : public Gtk::TreeModelColumnRecord
		{
		public:
			ModelColumns() {
				add(name);
				add(file_offset);
				add(virtual_addr);
			}

			Gtk::TreeModelColumn<Glib::ustring> name;
			Gtk::TreeModelColumn<Glib::ustring> file_offset;
			Gtk::TreeModelColumn<Glib::ustring> virtual_addr;
		};

		ModelColumns cols;
		Glib::RefPtr<Gtk::TreeStore> store;

	public:
		ProjectTreeView() {
			store = Gtk::TreeStore::create(cols);
			set_model(store);
			append_column("Name", cols.name);
			append_column("File Offset", cols.file_offset);
			append_column("Virtual Address", cols.virtual_addr);
		}

		void update(Sqlite& db) {
			store->clear();

			bool no_file_tree = true;
			bool no_group_tree = true;
			int64_t prev_file_id;
			std::string prev_group;
			
			Sqlite::Stmt stmt(db,
							  "SELECT file_id, filepath, part_group, name, "
							  "file_offset, virtual_addr "
							  "FROM part, file "
							  "WHERE part.file_id = file.rowid "
							  "ORDER BY filepath, part_group");

			Gtk::TreeModel::iterator i, j, k;
			while(db.step(stmt) == Sqlite::StepResult::ROW) {
				/* get columns */
				int64_t file_id = stmt.column_int(0);
				std::string file = stmt.column_text(1);
				std::string group = stmt.column_text(2);
				std::string name = stmt.column_text(3);
				int64_t file_offset = stmt.column_int(4);
				int64_t virtual_addr = stmt.column_int(5);

				/* check if switching to a new file subtree */
				if(no_file_tree || file_id != prev_file_id) {
					i = store->append();
					(*i)[cols.name] = file;
					prev_file_id = file_id;
					no_file_tree = false;
					no_group_tree = true;
				}

				/* check if switching to a new group subtree */
				if(no_group_tree || group != prev_group) {
					j = store->append(i->children());
					(*j)[cols.name] = group;
					prev_group = group;
					no_group_tree = false;
				}

				/* append new part */
				k = store->append(j->children());
				(*k)[cols.name] = name;
				(*k)[cols.file_offset] = hex(file_offset);
				(*k)[cols.virtual_addr] = hex(virtual_addr);
			}

			expand_all();
		}
	};
}

void App::init_ui_manager()
{
	Glib::RefPtr<Gtk::ActionGroup> group = Gtk::ActionGroup::create();
	Glib::RefPtr<Gtk::Action> action;

	group->add(Gtk::Action::create("FileMenu", "File"));
	
	group->add(Gtk::Action::create("NewProject", Gtk::Stock::NEW,
								   "New Project", "Create a new project"),
			   mem_fun(*this, &App::new_project));
	group->add(Gtk::Action::create("OpenProject", Gtk::Stock::OPEN,
								   "Open Project", "Open a project"),
			   mem_fun(*this, &App::open_project));
	group->add(Gtk::Action::create("SaveProject", Gtk::Stock::SAVE,
								   "Save Project", "Save the project"),
			   mem_fun(*this, &App::save_project));

	action = Gtk::Action::create("AddFile", Gtk::Stock::ADD,
								 "Add File", "Add a file to the project");
	group->add(action, mem_fun(*this, &App::add_file));

	/* create UI manager */
	ui_manager = Gtk::UIManager::create();
	ui_manager->insert_action_group(group);

	Glib::ustring ui_info = 
        "<ui>"
        "  <menubar name='MenuBar'>"
        "    <menu action='FileMenu'>"
        "      <menuitem action='NewProject'/>"
		"      <menuitem action='OpenProject'/>"
		"      <menuitem action='SaveProject'/>"
		"      <separator/>"
		"      <menuitem action='AddFile'/>"
        "    </menu>"
        "  </menubar>"
        "  <toolbar name='ToolBar'>"
        "    <toolitem action='NewProject'/>"
		"    <toolitem action='OpenProject'/>"
		"    <toolitem action='SaveProject'/>"
		"    <separator/>"
		"    <toolitem action='AddFile'/>"
        "  </toolbar>"
        "</ui>";

    ui_manager->add_ui_from_string(ui_info);
}

App::App(int argc, char **argv) : kit(argc, argv) {
	/* initialize window */
	window.set_default_size(1024, 768);

	/* vbox, tabs */
	auto box = new Gtk::Box(Gtk::ORIENTATION_VERTICAL);
	auto tabs = new Gtk::Notebook();

	/* create menubar/toolbar */
	init_ui_manager();
	box->add(*ui_manager->get_widget("/MenuBar"));
	box->add(*ui_manager->get_widget("/ToolBar"));

	/* project tree view */
	project_tree_view = new ProjectTreeView();
	auto scroll = new Gtk::ScrolledWindow();
	scroll->add(*project_tree_view);
	tabs->append_page(*scroll, "Project");

	/* hex view */
	scroll = new Gtk::ScrolledWindow();
	hex_view = new HexView();
	scroll->add(*hex_view);
	tabs->append_page(*scroll, "Hex");

	box->pack_end(*manage(tabs), true, true);
	window.add(*manage(box));
	window.show_all();
	set_project(nullptr);
	Gtk::Main::run(window);
}

void App::new_project() {
	set_project(new Project(":memory:"));
}

void App::open_project() {
	Gtk::FileChooserDialog dialog("Select project to open",
								  Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(window);
	
	auto db_filter = Gtk::FileFilter::create();
	db_filter->set_name("Sqlite3 Database");
	db_filter->add_pattern("*.sqlite3");
	dialog.add_filter(db_filter);
	
	auto all_filter = Gtk::FileFilter::create();
	all_filter->set_name("All Files");
	all_filter->add_pattern("*");
	dialog.add_filter(all_filter);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	if(dialog.run() == Gtk::RESPONSE_OK)
		set_project(new Project(dialog.get_filename()));
}

void App::save_project() {
	project->database().save("test.sqlite3");
}

void App::add_file() {
	Gtk::FileChooserDialog dialog("Select file to add",
								  Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(window);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	if(dialog.run() == Gtk::RESPONSE_OK) {
		project->add_file(dialog.get_filename());
		project_tree_view->update(project->database());
		update_hex_view();
	}
}

void App::set_project(Project* project_) {
	/* TODO: check if we need to close an old project */
	project = project_;

	if(project)
		window.set_title(project->name() + " - Odis");
	else
		window.set_title("Odis");
	
	ui_manager->get_action("/ToolBar/AddFile")->set_sensitive(project != nullptr);
	project_tree_view->set_sensitive(project != nullptr);
	//std::cout << ui_manager->get_ui() << std::endl;

	if(project) {
		project_tree_view->update(project->database());
		update_hex_view();
	}
}

void App::update_hex_view() {
	Sqlite::Stmt stmt(project->database(),
					  "SELECT data FROM file "
					  "WHERE rowid=?");
	/* TODO, need to be able to choose file */
	stmt.bind_int(1, 1);

	project->database().step(stmt);
	int size;
	const void* data = stmt.column_blob(0, &size);

	hex_view->set_data(data, size);
}

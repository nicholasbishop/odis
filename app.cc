#include "app.hh"
#include <iostream>

using namespace Odis;
using namespace sigc;

#if 0
GtkWidget *create_parts_treeview() {
	enum {
		GROUP,
		NAME,
		N_COLUMNS
	};

	GtkTreeStore *store = gtk_tree_store_new(N_COLUMNS,
											 G_TYPE_STRING,
											 G_TYPE_STRING);

	GtkTreeIter iter1, iter2;
	
	gtk_tree_store_append(store, &iter1, NULL);
	gtk_tree_store_set(store, &iter1,
					   GROUP, "Segments",
					   -1);

	gtk_tree_store_append(store, &iter2, &iter1);
	gtk_tree_store_set(store, &iter2,
					   NAME, "Test",
					   -1);

	gtk_tree_store_append(store, &iter1, NULL);
	gtk_tree_store_set(store, &iter1,
					   GROUP, "Sections",
					   -1);

	GtkWidget *tree;
	tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

	/* add columns */
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Group",
													  renderer,
													  "text", GROUP,
													  NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	column = gtk_tree_view_column_new_with_attributes("Name",
													  renderer,
													  "text", NAME,
													  NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

	/* open all by default */
	gtk_tree_view_expand_all(GTK_TREE_VIEW(tree));

	return tree;
}
#endif

namespace Odis {
	class ProjectTreeView : public Gtk::TreeView {
	private:
		class ModelColumns : public Gtk::TreeModelColumnRecord
		{
		public:
			ModelColumns() {
				add(file);
				add(group);
				add(name);
			}

			Gtk::TreeModelColumn<Glib::ustring> file;
			Gtk::TreeModelColumn<Glib::ustring> group;
			Gtk::TreeModelColumn<Glib::ustring> name;
		};

		ModelColumns cols;
		Glib::RefPtr<Gtk::TreeStore> store;

	public:
		ProjectTreeView() {
			store = Gtk::TreeStore::create(cols);
			set_model(store);
			append_column("File", cols.file);
			append_column("Group", cols.group);
			append_column("Name", cols.name);
		}

		void update(Sqlite& db) {
			store->clear();

			bool no_file_tree = true;
			bool no_group_tree = true;
			int64_t prev_file_id;
			std::string prev_group;
			
			std::vector<std::string> parts;
			Sqlite::Stmt stmt(db,
							  "SELECT file_id, filepath, part_group, name "
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

				/* check if switching to a new file subtree */
				if(no_file_tree || file_id != prev_file_id) {
					i = store->append();
					(*i)[cols.file] = file;
					prev_file_id = file_id;
					no_file_tree = false;
					no_group_tree = true;
				}

				/* check if switching to a new group subtree */
				if(no_group_tree || group != prev_group) {
					j = store->append(i->children());
					(*j)[cols.group] = group;
					prev_group = group;
					no_group_tree = false;
				}

				/* append new part */
				k = store->append(j->children());
				(*k)[cols.name] = name;
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
	group->add(Gtk::Action::create("SaveProject", Gtk::Stock::SAVE,
								   "Save Project", "Save the project"),
			   mem_fun(*this, &App::save_project));

	action = Gtk::Action::create("AddFile", Gtk::Stock::ADD,
								 "Add File", "Add a file to the project");
	action->set_sensitive(false);
	group->add(action, mem_fun(*this, &App::add_file));

	/* create UI manager */
	ui_manager = Gtk::UIManager::create();
	ui_manager->insert_action_group(group);

	Glib::ustring ui_info = 
        "<ui>"
        "  <menubar name='MenuBar'>"
        "    <menu action='FileMenu'>"
        "      <menuitem action='NewProject'/>"
		"      <menuitem action='SaveProject'/>"
		"      <separator/>"
		"      <menuitem action='AddFile'/>"
        "    </menu>"
        "  </menubar>"
        "  <toolbar name='ToolBar'>"
        "    <toolitem action='NewProject'/>"
		"    <toolitem action='SaveProject'/>"
		"    <separator/>"
		"    <toolitem action='AddFile'/>"
        "  </toolbar>"
        "</ui>";

    ui_manager->add_ui_from_string(ui_info);
}

App::App(int argc, char **argv) : kit(argc, argv) {
	/* initialize window */
	window.set_title("Odis");
	window.set_default_size(200, 200);
	Gtk::Box *box = new Gtk::Box(Gtk::ORIENTATION_VERTICAL);

	project_tree_view = new ProjectTreeView();

	/* create menubar/toolbar */
	init_ui_manager();
	box->add(*ui_manager->get_widget("/MenuBar"));
	box->add(*ui_manager->get_widget("/ToolBar"));

	Gtk::ScrolledWindow *scroll = new Gtk::ScrolledWindow();
	scroll->add(*project_tree_view);
	box->pack_end(*manage(scroll), true, true);

	//box.add(create_parts_treeview());
	window.add(*manage(box));

	window.show_all();
	Gtk::Main::run(window);
}

void App::new_project() {
	set_project(new Project(":memory:"));
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
	}
}

void App::set_project(Project* project_) {
	/* TODO: check if we need to close an old project */
	project = project_;

	window.set_title(project->name() + " - Odis");
	ui_manager->get_action("/ToolBar/AddFile")->set_sensitive(project != nullptr);
	//std::cout << ui_manager->get_ui() << std::endl;
	
}

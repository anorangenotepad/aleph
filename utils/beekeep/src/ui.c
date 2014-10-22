// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// gtk
#include <gtk/gtk.h>

/// bees
#include "net_protected.h"
#include "scene.h"

// beekeep
#include "ui.h"
#include "ui_handlers.h"
#include "ui_lists.h"
#include "ui_op_menu.h"

//----------------------------------
//--- variables

op_id_t newOpSelect = 0;

//----------------------------
//--- static functions

// make a scrollable box
static void scroll_box_new( ScrollBox* scrollbox, 
			    GtkWidget* parent, 
			    list_fill_fn fill,
			    list_select_fn select,
			    int x, int y, int w, int h)
{
  
  GtkWidget* scroll;
  GtkWidget* list;

  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scroll),
				   GTK_POLICY_AUTOMATIC, 
				  GTK_POLICY_AUTOMATIC);

  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 280);
  gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scroll), 280);
  // assume parent is a grid
  gtk_grid_attach(GTK_GRID(parent), scroll, x, y, w, h);

  list = gtk_list_box_new ();
  g_signal_connect (list, "row-selected", G_CALLBACK(select), NULL);

  (*fill)(GTK_LIST_BOX(list));

  gtk_container_add( GTK_CONTAINER(scroll), list );
  gtk_widget_show (list);

  scrollbox->scroll = GTK_SCROLLED_WINDOW(scroll);
  scrollbox->list = GTK_LIST_BOX(list);
}

// destroyer-iterator
static void destroy_iter(GtkWidget* wgt, gpointer data) {
  gtk_widget_destroy(wgt);
}

// remove all list elements
void scroll_box_clear( ScrollBox* scrollbox ) {
  gtk_container_foreach(GTK_CONTAINER(scrollbox), &(destroy_iter), NULL);
}

//--------------------
//--- selections

static void select_out( GtkListBox *box, gpointer data ) {
  // gotta be a better way to get this
  int id = gtk_list_box_row_get_index(gtk_list_box_get_selected_row(box));
  ui_select_out(id);
}

static void select_in( GtkListBox *box, gpointer data ) {
  int id = gtk_list_box_row_get_index(gtk_list_box_get_selected_row(box));
  ui_select_in(id);
}

static void select_op( GtkListBox *box, gpointer data ) {
  int id = gtk_list_box_row_get_index(gtk_list_box_get_selected_row(box));
  ui_select_op(id);
}

static void select_param( GtkListBox *box, gpointer data ) {
  int id = gtk_list_box_row_get_index(gtk_list_box_get_selected_row(box));
  ui_select_param(id);
}

static void scene_name_entry( GtkEntry *entry, gpointer data) {
  scene_set_name(gtk_entry_get_text(entry));
}

//------------------------
//---- variables

 ScrollBox boxOuts;
 ScrollBox boxIns;
 ScrollBox boxOps;
 ScrollBox boxParams;

// selected indexes
// negative == no selection
 int opSelect = -1;
 int outSelect = -1;
 int inSelect = -1;


//------------------------
//---- init, build, connect
void ui_init(void) {

  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *label;
  GtkWidget *opMenu;
  GtkWidget *wdg; // temp

  //---  window
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "bees editor");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
  /// need this?
  //  g_signal_connect (window, "delete-event", G_CALLBACK (delete_handler), NULL);
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  // grid layout
  grid = gtk_grid_new();
  gtk_grid_set_column_spacing (GTK_GRID(grid), 2);
  gtk_grid_set_row_spacing (GTK_GRID(grid), 2);
  gtk_container_add(GTK_CONTAINER(window), grid);

  // scene name
  wdg = gtk_entry_new();
  gtk_entry_set_text( GTK_ENTRY(wdg), scene_get_name() );
  gtk_grid_attach( GTK_GRID(grid), wdg, 0, 0, 1, 1 );
  g_signal_connect( wdg, "activate", G_CALLBACK(scene_name_entry), NULL);

  // export button
  
  // clear button

  // select module button (file dialog?)


  //  list labels
  label = gtk_label_new("OPERATORS");
  gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
  label = gtk_label_new("OUTPUTS");
  gtk_grid_attach(GTK_GRID(grid), label, 1, 1, 1, 1);
  label = gtk_label_new("INPUTS");
  gtk_grid_attach(GTK_GRID(grid), label, 2, 1, 1, 1);
  label = gtk_label_new("PARAMETERS");
  gtk_grid_attach(GTK_GRID(grid), label, 3, 1, 1, 1);

  //--- create scrolling list things 
  scroll_box_new( &boxOps, grid, &fill_ops, &select_op, 	0, 2, 1, 3 );
  scroll_box_new( &boxOuts, grid, &fill_outs, &select_out, 	1, 2, 1, 3);
  scroll_box_new( &boxIns, grid, &fill_ins, &select_in, 	2, 2, 1, 3 );
  scroll_box_new( &boxParams, grid, &fill_params, &select_param, 3, 2, 1, 3 );


  // new op menu
  opMenu = create_op_menu();
  wdg = gtk_menu_button_new();
  gtk_menu_button_set_popup( GTK_MENU_BUTTON(wdg), opMenu );
  gtk_grid_attach( GTK_GRID(grid), wdg, 0, 5, 1, 1 );
  
  /// show everything
  gtk_widget_show_all(window);
}

#include "CrystalControl.hpp"

#include <stdlib.h>
#include <pthread.h>
#include <ctime>
#include <iostream>

static gboolean
c_run_some_steps(gpointer data)
{
  return ((CrystalControl *)data)->run_some_steps();
}
#define C_CB_FN(FN) c_##FN
#define CLICK_CB_DEF(FN, T)			\
  static gboolean				\
  C_CB_FN(FN)(GtkWidget *widget,			\
	    gpointer data)			\
  { (void)widget;				\
    return ((T)data)->FN();			\
  }

CLICK_CB_DEF(change_speed_event_cb, CrystalControl *)
CLICK_CB_DEF(start_event_cb, CrystalControl *)
CLICK_CB_DEF(stop_event_cb, CrystalControl *)

static double
get_time() {
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t.tv_sec + t.tv_nsec/1e9;
}

static void
show_error_message(GtkWindow *parent,
		   char const *msg)
{
  GtkWidget *dialog = gtk_message_dialog_new(parent,
					     GTK_DIALOG_DESTROY_WITH_PARENT,
					     GTK_MESSAGE_WARNING,
					     GTK_BUTTONS_OK,
					     msg, NULL);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

CrystalControl::CrystalControl(std::shared_ptr<CrystalModel> cm,
			       std::shared_ptr<CrystalView> cv)
  : m_cm(cm),
    m_cv(cv)
{
}

CrystalControl::~CrystalControl()
{
  signal_stop_simulation();
}

uint32_t
CrystalControl::request_new_modifier()
{
  GtkDialogFlags flags = (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT);
  GtkWidget *dialog = gtk_dialog_new_with_buttons("Change Speed",
						  GTK_WINDOW(m_main_app_window),
						  flags,
						  ("OK"), GTK_RESPONSE_ACCEPT,
						  ("Cancel"), GTK_RESPONSE_REJECT,
						  NULL);
  GtkWidget *dialog_content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget *new_mod_tf = gtk_entry_new();
  gtk_entry_set_input_purpose(GTK_ENTRY(new_mod_tf), GTK_INPUT_PURPOSE_DIGITS);
  gtk_box_pack_start(GTK_BOX(dialog_content),
		     gtk_label_new("Enter a new speed:"),
		     FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(dialog_content),
		     new_mod_tf,
		     FALSE, FALSE, 0);

  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
		    dialog_content);
  gtk_widget_show_all(dialog);
  
  int new_speed = -1;
  for (bool input_accepted = false; !input_accepted;) {
    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    if (result == GTK_RESPONSE_ACCEPT) {
      if (gtk_entry_get_text_length(GTK_ENTRY(new_mod_tf)) > 0) {
	int _new_speed = atoll(gtk_entry_get_text(GTK_ENTRY(new_mod_tf)));
	if (_new_speed > 0) {
	  new_speed = _new_speed;
	  input_accepted = true;
	} else {
	  show_error_message(GTK_WINDOW(m_main_app_window),
			     "New value must a positive integer.");
	  gtk_entry_set_text(GTK_ENTRY(new_mod_tf), "");
	}
      }
    } else {
      input_accepted = true;
    }
  }
  gtk_widget_destroy(dialog);
  return new_speed;
}

void
CrystalControl::signal_stop_simulation()
{
  if (sim_running) {
    sim_running = false;
  }
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

void
CrystalControl::run_thread()
{
  double t0 = get_time();
  while (sim_running) {
    if (!m_cm->run_some_steps(number)) {
      sim_running = false;
    }
    g_timeout_add(0, &c_run_some_steps, this);
  }
  double t1 = get_time();
  std::cout << "C++ Simulation took " << (t1-t0) << " s" << std::endl;
}

gboolean
CrystalControl::run_some_steps()
{
  m_cv->repaint();
  return FALSE; // run once
}

gboolean
CrystalControl::change_speed_event_cb()
{
  signal_stop_simulation();
  
  int modifier = request_new_modifier();
  if (modifier > 0) {
    number = modifier;
  }
  return TRUE;
}

gboolean
CrystalControl::start_event_cb()
{
  signal_stop_simulation();
  
  m_cm->reset();
  m_cv->repaint();
  
  if (!sim_running) {
    //m_cm->srand(time(NULL));
    m_cm->srand(0);
    sim_running = true;
    m_thread = std::thread(&CrystalControl::run_thread, this);
  }
  return TRUE;
}


gboolean
CrystalControl::stop_event_cb()
{
  signal_stop_simulation();
  return TRUE;
}

void
CrystalControl::init_ui(GtkBuilder *builder)
{
  m_cv->set_gtk_widget(GTK_WIDGET(gtk_builder_get_object(builder, "bath")));
  
  GtkWidget *start_btn = GTK_WIDGET(gtk_builder_get_object(builder, "start_btn"));
  g_signal_connect(start_btn, "clicked", G_CALLBACK(C_CB_FN(start_event_cb)), this);
  GtkWidget *stop_btn = GTK_WIDGET(gtk_builder_get_object(builder, "stop_btn"));
  g_signal_connect(stop_btn, "clicked", G_CALLBACK(C_CB_FN(stop_event_cb)), this);
  GtkWidget *change_speed_btn = GTK_WIDGET(gtk_builder_get_object(builder, "chspeed_btn"));
  g_signal_connect(change_speed_btn, "clicked", G_CALLBACK(C_CB_FN(change_speed_event_cb)), this);
  
  m_main_app_window = GTK_WIDGET(gtk_builder_get_object(builder, "app-window"));
}

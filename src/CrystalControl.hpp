#ifndef CRYSTALCONTROL_H_
#define CRYSTALCONTROL_H_

#include <gtk/gtk.h>
#include <thread>
#include <memory>
#include "CrystalModel.hpp"
#include "CrystalView.hpp"

#define DEFAULT_SPEED 10

class CrystalControl
{
public:
  CrystalControl(std::shared_ptr<CrystalModel> cm,
		 std::shared_ptr<CrystalView> cv);
  ~CrystalControl();
  void
  init_ui(GtkBuilder *builder);
  
  gboolean
  run_some_steps();
  gboolean
  change_speed_event_cb();
  gboolean
  start_event_cb();
  gboolean
  stop_event_cb();
private:
  std::shared_ptr<CrystalModel> m_cm;
  std::shared_ptr<CrystalView> m_cv;
  uint32_t number = DEFAULT_SPEED;

  GtkWidget *m_main_app_window;
  
  volatile bool sim_running;
  std::thread m_thread;

  uint32_t
  request_new_modifier();
  void
  signal_stop_simulation();
  void
  run_thread();
};

#endif //CRYSTALCONTROL_H_

#ifndef CRYSTAL_VIEW_H_
#define CRYSTAL_VIEW_H_

#include <gtk/gtk.h>
#include <memory>
#include "CrystalModel.hpp"

class CrystalView
{
public:
  CrystalView(std::shared_ptr<CrystalModel> cm);
  ~CrystalView();
  
  void
  repaint();
  void
  set_gtk_widget(GtkWidget *widget);
  gboolean
  draw_cb(cairo_t *cr);
  gboolean
  configure_event_cb(GtkWidget *widget);
private:
  std::shared_ptr<CrystalModel> m_cm;
  cairo_surface_t *m_surface = NULL;
  GtkWidget *m_canvas = NULL;

  cairo_t *m_cr = NULL;
  int m_width;
  int m_height;

  bool m_scale_required = false;
  double m_x_scale = 1;
  double m_y_scale = 1;

  void
  draw_background();
  void
  draw_edge();
  void
  draw_crystal();
};

#endif //CRYSTAL_VIEW_H_

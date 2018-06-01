#include "CrystalView.hpp"

#include <math.h>
#include <stdlib.h>

static gboolean
c_draw_cb(GtkWidget *widget,
	  cairo_t *cr,
	  gpointer data)
{
  (void)widget;
  if (!data) { return FALSE; }
  return ((CrystalView *)data)->draw_cb(cr);
}
static gboolean
c_configure_event_cb(GtkWidget *widget,
		     GdkEventConfigure *event,
		     gpointer data)
{
  (void)event;
  if (!data) { return FALSE; }
  return ((CrystalView *)data)->configure_event_cb(widget);
}

CrystalView::CrystalView(std::shared_ptr<CrystalModel> cm)
  : m_cm(cm),
    m_canvas(nullptr),
    m_width(cm->get_bath_width()),
    m_height(m_cm->get_bath_width())
{
}

CrystalView::~CrystalView()
{
  cairo_destroy(m_cr);
  cairo_surface_destroy(m_surface);
}

void
CrystalView::repaint()
{
  gtk_widget_queue_draw(m_canvas);
}

void
CrystalView::set_gtk_widget(GtkWidget *widget)
{
  m_canvas = widget;
  gtk_widget_set_size_request(m_canvas, m_cm->get_bath_width(), m_cm->get_bath_width());
  
  g_signal_connect(m_canvas, "draw", G_CALLBACK(c_draw_cb), this);
  g_signal_connect(m_canvas, "configure-event", G_CALLBACK(c_configure_event_cb), this);
}

void
CrystalView::draw_background()
{
  cairo_set_source_rgb(m_cr, 0, 0, 0);
  cairo_paint(m_cr);
}

void
CrystalView::draw_edge()
{
  int x_c = m_cm->x_bath_to_model_rep(0);
  int y_c = m_cm->y_bath_to_model_rep(0);
  
  cairo_set_line_width(m_cr, 1);
  
  cairo_arc(m_cr, x_c, y_c, m_cm->get_radius(), 0, 2*M_PI);
  cairo_set_source_rgb(m_cr, 0, 0, 1);
  cairo_stroke(m_cr);
  
  cairo_arc(m_cr, x_c, y_c, m_cm->get_r_bounds(), 0, 2*M_PI);
  cairo_set_source_rgb(m_cr, 1, 1, 0);
  cairo_stroke(m_cr);
}

void
CrystalView::draw_crystal()
{
  int x_l = m_cm->get_x();
  int y_l = m_cm->get_y();
  int r = m_cm->get_radius();
  
  cairo_set_line_width(m_cr, 0);
  for(int i = -r; i <= r; i++) {
    for(int j = -r; j <= r; j++) {
      if (m_cm->get_model_value(i, j) == BATH_POPULATED) {
	cairo_rectangle(m_cr,
			m_cm->x_bath_to_model_rep(i),
			m_cm->y_bath_to_model_rep(j),
			1, 1);
      }
    }
  }
  cairo_set_source_rgb(m_cr, 1, 0, 0);
  cairo_fill(m_cr);
  
  cairo_rectangle(m_cr,
		  m_cm->x_bath_to_model_rep(x_l),
		  m_cm->y_bath_to_model_rep(y_l),
		  1, 1);
  cairo_set_source_rgb(m_cr, 0, 1, 0);
  cairo_fill(m_cr);
}

gboolean
CrystalView::draw_cb(cairo_t *cr)
{
  draw_background();
  draw_edge();
  draw_crystal();

  if (m_scale_required) {
    cairo_scale(cr, m_x_scale, m_y_scale);
  }
  cairo_set_source_surface(cr, m_surface, 0, 0);
  cairo_paint(cr);
  
  return FALSE;
}

gboolean
CrystalView::configure_event_cb(GtkWidget *widget)
{
  int width = gtk_widget_get_allocated_width(widget);
  int height = gtk_widget_get_allocated_height(widget);
  m_scale_required = width != m_width || height != m_height;
  m_x_scale = (double)width / (double)m_width;
  m_y_scale = (double)height / (double)m_height;

  if (m_surface) {
    return TRUE;
  }
  m_surface = gdk_window_create_similar_surface(gtk_widget_get_window(m_canvas),
						CAIRO_CONTENT_COLOR,
						m_width,
						m_height);
  m_cr = cairo_create(m_surface);
  
  return TRUE;
}

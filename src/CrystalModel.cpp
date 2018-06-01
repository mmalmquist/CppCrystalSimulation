#include "CrystalModel.hpp"

#include <cmath>
#include <sstream>
#include <string>

#include "random.h"

static std::string const VBOUND = "|";
static std::string const HBOUND = "-";
static std::string const EMPTY = " ";
static std::string const POPULATED = "*";
static std::string const POPULATED_LAST = "#";

static int const dx[] = { 1, -1, 0, 0 };
static int const dy[] = { 0, 0, 1, -1 };

static inline void step_once(Point<int> &p) {
  int i = cs_rand() & 3;
  p.x() += dx[i];
  p.y() += dy[i];
}

CrystalModel::CrystalModel(int bath_width, int r_start, int r_escape) :
  m_mat(bath_width),
  m_r_start(r_start),
  m_r_escape(r_escape),
  m_bath_width(bath_width)
{
  reset();
}

void CrystalModel::srand(int seed) {
  cs_srand(seed);
}

bool CrystalModel::crystallize_one_ion() {
  Point<int> p;
  for (drop_new_ion(m_r_start, p); !any_neighbours(p); step_once(p)) {
    if (outside_circle(m_r_escape, p)) {
      return crystallize_one_ion();
    }
  }
  m_p = p;
  bath_at(m_p.x(), m_p.y()) = BATH_POPULATED;
  return !outside_circle(m_r_start, m_p);
}

void CrystalModel::reset()
{
  m_mat.clear();
  bath_at(0, 0) = BATH_POPULATED;
}

bool CrystalModel::run_some_steps(int steps)
{
  if (steps > 0) {
    return crystallize_one_ion() && run_some_steps(steps-1);
  }
  return true;
}

std::string CrystalModel::to_string() const
{
  int x = get_x();
  int y = get_y();
  int size = get_radius();
  std::ostringstream os;
  for(int i = -(size+1); i < size+1; i++) {
    os << HBOUND;
  }
  os << "\n";
  for(int i = -size; i < size; i++) {
    os << VBOUND;
    for(int j = -size; j < size; j++) {
      if (get_model_value(i, j)) {
	os << ((i == x && j == y) ? POPULATED_LAST : POPULATED);
      } else {
	os << EMPTY;
      }
    }
    os << VBOUND;
    os << "\n";
  }
  for(int i = -(size+1); i < size+1; i++) {
    os << HBOUND;
  }
  os << "\n";
  return os.str();
}

bool CrystalModel::any_neighbours(Point<int> const &p) const
{
  return get_model_value(p.x()+1, p.y())
    || get_model_value(p.x()-1, p.y())
    || get_model_value(p.x(), p.y()+1)
    || get_model_value(p.x(), p.y()-1);
}

void CrystalModel::drop_new_ion(int r, Point<int> &p) const
{
  double alpha = 2 * M_PI * cs_drand();
  p.x() = (int)(r*cos(alpha));
  p.y() = (int)(r*sin(alpha));
}

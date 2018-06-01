#ifndef CRYSTAL_MODEL_H_
#define CRYSTAL_MODEL_H_

#include <string>
#include <stdlib.h>

#include "Point.hpp"
#include "Matrix.hpp"

#define BATH_TYPE int
#define BATH_POPULATED ((int)1)
#define BATH_NOT_POPULATED ((int)0)

class CrystalModel
{
public:
  CrystalModel(int bath_width, int r_start, int r_escape);
  bool crystallize_one_ion();
  void reset();
  bool run_some_steps(int steps);
  std::string to_string() const;
  inline BATH_TYPE get_model_value(int x, int y) const { return bath_at(x, y); }
  inline int x_bath_to_model_rep(int x) const { return x + m_bath_width/2; } 
  inline int y_bath_to_model_rep(int y) const { return m_bath_width/2 - y; } 
  inline int get_x() const { return m_p.x(); }
  inline int get_y() const { return m_p.y(); }
  inline int get_r_bounds() const { return m_r_start; }
  inline int get_radius() const { return m_r_escape; }
  inline int get_bath_width() const { return m_bath_width; }
  void srand(int seed);
private:
  Matrix<BATH_TYPE> m_mat;
  Point<int> m_p;
  int m_r_start;
  int m_r_escape;
  int m_bath_width;
  
  bool any_neighbours(Point<int> const &p) const;
  void drop_new_ion(int r, Point<int> &p) const;

  inline bool outside_circle(int r_escape, Point<int> const &p) const { return p.length() >= r_escape; }

  inline BATH_TYPE &bath_at(int x, int y) {
    return m_mat.at(x_bath_to_model_rep(x), y_bath_to_model_rep(y));
  }

  inline BATH_TYPE const &bath_at(int x, int y) const {
    return m_mat.at(x_bath_to_model_rep(x), y_bath_to_model_rep(y));
  }
};

#endif //CRYSTAL_MODEL_H_

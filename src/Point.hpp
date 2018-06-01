#ifndef POINT_H_
#define POINT_H_

#include <cmath>

template<typename T>
class Point
{
public:
  Point(T x=T(0), T y=T(0)) : m_x(x), m_y(y) { }
  inline T &x() { return m_x; }
  inline T const &x() const { return m_x; }
  inline T &y() { return m_y; }
  inline T const &y() const { return m_y; }
  inline T length() const { return T(sqrt(m_x*m_x + m_y*m_y)); }
  inline Point<T> operator+(Point<T> const &other) const { return Point<T>(m_x + other.m_x, m_y + other.m_y); }
  inline Point<T> &operator+=(Point<T> const &other) { m_x += other.m_x; m_y += other.m_y; return *this; }
  
private:
  T m_x;
  T m_y;
};

#endif // POINT_H_

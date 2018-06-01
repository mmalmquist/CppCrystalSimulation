#ifndef MATRIX_H_
#define MATRIX_H_

#include <vector>
#include <inttypes.h>

template<typename T>
class Matrix
{
public:
  Matrix(uint32_t size) : m_array(size*size), m_size(size) { }

  void clear()
  {
    for (typename std::vector<T>::iterator itr = m_array.begin(); itr != m_array.end(); ++itr) {
      *itr = T(0);
    }
  }

  inline T &at(int x, int y) { return m_array[x + y*m_size]; }
  inline T const &at(int x, int y) const { return m_array[x + y*m_size]; }
  
private:
  std::vector<T> m_array;
  uint32_t m_size;
};

#endif // MATRIX_H_

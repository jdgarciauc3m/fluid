#ifndef FLUID_DOMAIN_H
#define FLUID_DOMAIN_H

#include "params.h"
#include <yapl/cube_index.h>
#include <cassert>

namespace fluid {

template <typename T>
class domain {
public:
  domain(T h);

  template <int D>
  size_t upper_index() const;

  yapl::cube_index grid_position(const space_vector<T> & p) const;

  const yapl::cube_index size_;
  const size_t num_cells_;
  const space_vector<T> delta_;
};

template <typename T>
domain<T>::domain(T h)
:
size_{space_vector<size_t>{constants::DOMAIN_RANGE<T>() / h}},
num_cells_{size_.volume()},
delta_{constants::DOMAIN_RANGE<T>() / space_vector<T>(size_)}
{
  //assert(size_ >= (yapl::cube_index{1,1,1}));
  //assert(delta_ >= params<T>::h());
}

template <typename T>
template <int D>
size_t domain<T>::upper_index() const
{
  return size_.template get<D>() - 1;
}

template <typename T>
yapl::cube_index domain<T>::grid_position(const space_vector<T> & p) const
{
  using namespace constants;
  space_vector<int> i { (p - DOMAIN_MIN<T>()) / delta_ };
  return size_.box(i);
}

}

#endif

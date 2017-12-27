#ifndef FLUID_POLICY_H
#define FLUID_POLICY_H

#include "cell.h"
#include <yapl/policy.h>
#include <yapl/tbbexecutor.h>

namespace fluid {

template <typename T, bool cfl>
struct sequential_policy {
  using cell_type = cell<T, null_mutex, cfl>;
  using grid_policy = yapl::default_policy<cell_type>;
};

template <typename T, bool cfl>
struct tbb_policy {
  using cell_type = cell<T, spin_mutex, cfl>;
  using grid_policy = yapl::policy<yapl::tbb_executor<cell_type>>;
};

}

#endif

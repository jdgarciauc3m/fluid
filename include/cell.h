#ifndef FLUID_CELL_H
#define FLUID_CELL_H

#include "particle.h"
#include <yapl/cube.h>
#include <yapl/policy.h>
#include <tbb/tbb.h>
#include <tbb/scalable_allocator.h>
#include <vector>
#include <mutex>
#include <type_traits>

#include <iostream>
#include <cassert>

namespace fluid {

class null_checker {
public:
  using index_type = yapl::cube_index;

  null_checker() = default;

  void set_index(const index_type &) {}

  void check(const index_type &) const {}
};

class cfl_checker {
public:
  using index_type = yapl::cube_index;

  cfl_checker() : position_{0,0,0} {}

  void set_index(const index_type & i) { position_ = i; }

  void check(const index_type & i) const; 

private:
  index_type position_;
};

void cfl_checker::check(const index_type & i) const
{
  space_vector<int> diff = i;
  diff -= space_vector<size_t>(position_.get<0>(), position_.get<1>(), position_.get<2>());
  if (std::abs(diff.x())>1 || std::abs(diff.y())>1 || std::abs(diff.z())>1) {
    throw std::logic_error("FATAL ERROR: Courant–Friedrichs–Lewy condition not satisfied.");
  }
}

class null_mutex {
public:
  void lock() {}
  bool try_lock() { return true; }
  void unlock() {}
};

class spin_mutex {
public:
  void lock() { mtx_.lock(); }
  bool try_lock() { return mtx_.try_lock(); }
  void unlock() { mtx_.unlock(); }
private:
  tbb::spin_mutex mtx_;
};

template <typename T, typename M, bool CFL>
class cell : public std::conditional<CFL,cfl_checker,null_checker>::type {
public:
  using checker_type = typename std::conditional<CFL, cfl_checker, null_checker>::type;
  using index_type = yapl::cube_index;

  cell();

  cell(const cell & c) = delete;
  cell & operator=(const cell & c) = delete;

  cell(cell && c) = delete;
  cell & operator=(cell && c) = delete;

  void add_neighbour(cell<T,M,CFL> & c);

  void clear_particles();
  void add_particle(const space_vector<T> & p, const space_vector<T> & hv, const space_vector<T> & v);
  void add_particle(const particle<T> & p) { 
    using namespace std;
    lock_guard<M> l{mutex_};
    particles_.push_back(p); 
  }
  size_t num_particles() const { 
    using namespace std;
    lock_guard<M> l{mutex_};
    return particles_.size(); 
  }

  template <typename F>
  void for_all_particles(F f) {
    using namespace std;
    lock_guard<M> l{mutex_};
    for (auto & p : particles_) { f(p); }
  }

  template <typename F>
  void for_all_particles(F f) const {
    using namespace std;
    lock_guard<M> l{mutex_};
    for (auto & p : particles_) { f(p); }
  }

  template <typename F>
  void for_all_near_particles(F f);

  template <class OS>
  friend OS & operator<<(OS & os, const cell & c) {
    using namespace std;
    lock_guard<M> l{c.mutex_};
    for (auto & p : c.particles_) {
      os << p << std::endl;
    }
    return os;
  }

  // Checking interface
  void set_index(const index_type & i) { checker_type::set_index(i); }
  void check(const index_type & i) const {checker_type::check(i); }
  
protected:
  std::vector<particle<T>> particles_;
  std::vector<cell<T,M,CFL>*> neighbours_;
  mutable M mutex_;
};

template <typename T, typename M, bool CFL>
cell<T,M,CFL>::cell()
:
particles_{},
neighbours_{}
{
  neighbours_.reserve(13);
}

template <typename T, typename M, bool CFL>
void cell<T,M,CFL>::add_neighbour(cell<T,M,CFL> & c) 
{
  neighbours_.push_back(&c);
}

template <typename T, typename M, bool CFL>
void cell<T,M,CFL>::clear_particles()
{
  using namespace std;
  lock_guard<M> l{mutex_};
  particles_.clear();
  particles_.shrink_to_fit();
}

template <typename T, typename M, bool CFL>
void cell<T,M,CFL>::add_particle(const space_vector<T> & p, const space_vector<T> & hv, const space_vector<T> & v)
{
  using namespace std;
  lock_guard<M> l{mutex_};
  particles_.emplace_back(p,hv,v);
}

template <typename T, typename M, bool CFL>
template <typename F>
void cell<T,M,CFL>::for_all_near_particles(F f)
{
  using namespace std;
  auto begin = particles_.begin();
  auto end = particles_.end();
  for (auto i=begin; i!=end; ++i) {
    {
      lock_guard<M> l{mutex_};
      for (auto j=begin; j!=i; ++j) {
        f(*i,*j);
      }
    }
    
    for (auto & nc : neighbours_) {
      lock(mutex_, nc->mutex_);
      for (auto & np : nc->particles_) {
        f(*i,np);
      }
      mutex_.unlock();
      nc->mutex_.unlock();
    };
  }
}


}

#endif

#ifndef FLUID_GRID_H
#define FLUID_GRID_H

#include "domain.h"
#include "params.h"
#include "cell.h"
#include "simulation_stream.h"
#include "policy.h"
#include <yapl/cube.h>
#include <yapl/algorithm.h>
#include <iostream>
#include <algorithm>
#include <numeric>

namespace fluid {

template <typename T, typename P>
class grid {
public:
  grid(T ppm);

  grid(const grid & g) = delete;
  grid & operator=(const grid &) = delete;
  
  grid(grid && g) = delete;
  grid & operator=(grid && g) = delete;

  size_t num_cells() const { return domain_.num_cells_; }

  void rebuild_grid();
  void compute_forces();
  void process_collisions();
  void reprocess_collisions();
  void advance_particles();

  void get_statistics(float & m, float & d, size_t & nempty) const;

  void read(simulation_istream & is, size_t np);
  void write(simulation_ostream & os) const;

private:

  template <int I>
  void do_process_collisions_lower();

  template <int I>
  void do_reprocess_collisions_upper();

  template <int I>
  void do_reprocess_collisions_lower();

  template <int I>
  void do_process_collisions_upper();

private:

  const params<T> params_;
  const domain<T> domain_;

  using cell_type = typename P::cell_type;
  using grid_policy = typename P::grid_policy;
  using cube_type = yapl::cube<cell_type, grid_policy>;

  cube_type cells_;
  cube_type cells2_;
};


template <typename T, typename P>
grid<T,P>::grid(T ppm)
:
params_{ppm},
domain_{params_.h_},

cells_{domain_.size_},
cells2_{domain_.size_}
{
  yapl::apply_indexed(cells_.all(), [this](cell_type & c, const yapl::cube_index & i) {
    c.set_index(i);
    cells_.for_all_neighbours_unique(i, [&c](cell_type & nc) {
      c.add_neighbour(nc);
    });
  });
  yapl::apply_indexed(cells2_.all(), [this](cell_type & c, const yapl::cube_index & i) {
    c.set_index(i);
    cells2_.for_all_neighbours_unique(i, [&c](cell_type & nc) {
      c.add_neighbour(nc);
    });
  });
}

template <typename T, typename P>
void grid<T,P>::rebuild_grid()
{
  //swap src and dest arrays with particles
  yapl::swap(cells_,cells2_);

  yapl::apply(cells_.all(), [](cell_type & c) {
    c.clear_particles();
  });

  // Reposition particles in corresponding cell
  yapl::apply(cells2_.all(), [this](const cell_type & vc) {
    vc.for_all_particles([this,&vc](const particle<T> & p) {
      auto i = p.grid_position(domain_);
      vc.check(i);
      cells_(i).add_particle(p);
    });
 });
}


template <typename T, typename P>
void grid<T,P>::process_collisions()
{
  do_process_collisions_lower<0>();
  do_process_collisions_upper<0>();
  do_process_collisions_lower<1>();
  do_process_collisions_upper<1>();
  do_process_collisions_lower<2>();
  do_process_collisions_upper<2>();
}

// Notes on USE_ImpeneratableWall
// When particle is detected beyond cell wall it is repositioned at cell wall
// velocity is not changed, thus conserving momentum.
// What this means though it the prior AdvanceParticles had positioned the
// particle beyond the cell wall and thus the visualization will show these
// as artifacts. The proper place for USE_ImpeneratableWall is after AdvanceParticles.
// This would entail a 2nd pass on the perimiters after AdvanceParticles (as opposed
// to inside AdvanceParticles). Your fluid dynamisist should properly devise the
// equasions. 
//
// N.B. The integration of the position can place the particle
// outside the domain. We now make a pass on the perimiter cells
// to account for particle migration beyond domain.
template <typename T, typename P>
void grid<T,P>::reprocess_collisions()
{
#ifdef USE_ImpeneratableWall
  do_reprocess_collisions_lower<0>();
  do_reprocess_collisions_upper<0>();
  do_reprocess_collisions_lower<1>();
  do_reprocess_collisions_upper<1>();
  do_reprocess_collisions_lower<2>();
  do_reprocess_collisions_upper<2>();
#endif
}

template <typename T, typename P>
void grid<T,P>::advance_particles()
{
  yapl::apply(cells_.all(), [](cell_type & c) {
    c.for_all_particles([](particle<T> & p) {
      p.advance();
    });
  });
}

template <typename T, typename P>
void grid<T,P>::read(simulation_istream & is, size_t np)
{
  space_vector<T> position, hv, velocity;
  for(size_t i = 0; i < np; ++i)
  {
    // Read position, hv and velocity
    position = is.read_space_vector<T>();
    hv = is.read_space_vector<T>();
    velocity = is.read_space_vector<T>();

    // Add to cell of position in domain
    cells_(domain_.grid_position(position)).add_particle(position, hv, velocity);
  }
}

template <typename T, typename P>
void grid<T,P>::write(simulation_ostream & os) const
{
  yapl::apply(cells_.all_ordered(), [&os](const cell_type & c) {
    c.for_all_particles([&os](const particle<T> & p) {
      p.write(os);
    });
  });

}

// Precondition: All particles have density = 0
// Precondition: All particles have acceleration = externalAcceleration
template <typename T, typename P>
void grid<T,P>::compute_forces()
{
  // Increase densities
  yapl::apply(cells_.all(), 
    [this](cell_type & c) {
      c.for_all_near_particles([this](particle<T> & p1, particle<T> & p2) {
        p1.increase_densities(p2, params_.hsq_);
      });
    }
  );

  // Transform densities
  yapl::apply(cells_.all(),
    [this](cell_type & c) {
      c.for_all_particles([this](particle<T> & p) {
        p.transform_density(params_.density_coeff_,params_.h6_);
      });
    }
  );

  // Transfer accelerations
  yapl::apply(cells_.all(),
    [this](cell_type & c) {
      c.for_all_near_particles([this](particle<T> & p1, particle<T> & p2) {
        p1.transfer_acceleration(p2, params_.h_, params_.hsq_,
          params_.pressure_coeff_, params_.viscosity_coeff_);
      });
    }
  );
}

template <typename T, typename P>
void grid<T,P>::get_statistics(float & m, float & v, size_t & nempty) const
{
  std::vector<size_t> count;
  nempty = 0;
  apply(cells_.all_ordered(), [&count, &nempty](cell_type & c) {
    size_t num = c.num_particles();
    count.push_back(num);
    if (num>0) nempty++;
  });
  size_t n = std::accumulate(count.begin(), count.end(), 0);
  m = static_cast<float>(n) / domain_.num_cells_;

  v = 0.0;
  for (auto c : count) {
    v+= (c-m) * (c-m);
  }
  v = std::sqrt(v);
}

template <typename T, typename P>
template <int I>
void grid<T,P>::do_process_collisions_lower()
{
  yapl::apply(cells_.template plane<I>(0), 
    [](cell_type & c) { 
      c.for_all_particles([](particle<T> & p) {
        p.template process_collision_lower<I>();
      });
    });
}

template <typename T, typename P>
template <int I>
void grid<T,P>::do_process_collisions_upper()
{
  auto upper = domain_.template upper_index<I>();
  yapl::apply(cells_.template plane<I>(upper), 
    [](cell_type & c) { 
      c.for_all_particles([](particle<T> & p) {
        p.template process_collision_upper<I>();
      });
    });
}

template <typename T, typename P>
template <int I>
void grid<T,P>::do_reprocess_collisions_lower()
{
  yapl::apply(cells_.template plane<I>(0), 
    [](cell_type & c) { 
      c.for_all_particles([](particle<T> & p) {
        p.template reprocess_collision_lower<I>();
      });
    });
}

template <typename T, typename P>
template <int I>
void grid<T,P>::do_reprocess_collisions_upper()
{
  auto upper = domain_.template upper_index<I>();
  yapl::apply(cells_.template plane<I>(upper), 
    [](cell_type & c) { 
      c.for_all_particles([](particle<T> & p) {
        p.template reprocess_collision_upper<I>();
      });
    });
}


}

#endif

#ifndef FLUID_SIMULATION_H
#define FLUID_SIMULATION_H

#include "grid.h"

namespace fluid {

template <typename T, typename P>
class simulation {
public:
  simulation(T ppm, size_t np);

  size_t num_cells() const { return grid_.num_cells(); }

  void advance_frame();

  void read(simulation_istream & is) { grid_.read(is, num_particles_); }
  void write(simulation_ostream & os) const;

  void print_statistics() const;

private:
  const T particles_per_meter_;
  const size_t num_particles_;

  grid<T,P> grid_;
};


template <typename T, typename P>
simulation<T,P>::simulation(T ppm, size_t np)
:
particles_per_meter_{ppm},
num_particles_{np},
grid_{ppm}
{
}

template <typename T, typename P>
void simulation<T,P>::advance_frame()
{
  grid_.rebuild_grid();
  grid_.compute_forces();
  grid_.process_collisions();
  grid_.advance_particles();
  grid_.reprocess_collisions();
  print_statistics();
}

template <typename T, typename P>
void simulation<T,P>::write(simulation_ostream & os) const
{
  os.write_header(particles_per_meter_, num_particles_);
  grid_.write(os);
}

template <typename T, typename P>
void simulation<T,P>::print_statistics() const
{
#ifdef ENABLE_STATISTICS
  float mean, stddev;
  size_t nempty;
  grid_.get_statistics(mean,stddev,nempty);
  std::cout << "cell statistics: mean=" << mean << " particles, stddev=" << stddev << " particles." << std::endl;
  std::cout << "Empty cells: " << nempty << std::endl;
#endif
}

}

#endif

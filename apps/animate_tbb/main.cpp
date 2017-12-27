#ifdef ENABLE_VISUALIZATION
static_assert(false, "Visualization not implemented");
#endif

#include "simulation_stream.h"
#include "simulation.h"
#include "policy.h"
#include <xul/time_meter/optional_meter.h>
#include <xul/time_meter/system_meter.h>
#include <iostream>

void cfl_warn()
{
#ifdef ENABLE_CFL_CHECK
  std::cout << "WARNING: Check for Courant–Friedrichs–Lewy condition enabled. Do not use for performance measurements." << std::endl;
#endif
}

#ifdef ENABLE_DOUBLE_PRECISION
using data_type = double;
#else
using data_type = float;
#endif

#ifdef ENABLE_CFL_CHECK
constexpr bool cfl_check = true;
#else
constexpr bool cfl_check = false;
#endif

using policy_type = fluid::tbb_policy<data_type,cfl_check>;
using simulation_type = fluid::simulation<data_type, policy_type>;

int main(int argc, char *argv[])
{
  if(argc < 4 || argc >5)
  {
    std::cerr << "Usage: " << argv[0] << " <threadnum> <framenum> <.fluid input file> [.fluid output file]" << std::endl;
    return -1;
  }

  int threadnum = std::stoi(argv[1]);
  int framenum = std::stoi(argv[2]);

  //Check arguments
  /*
  if(threadnum != 1) {
    std::cerr << "<threadnum> must be 1 (serial version)" << std::endl;
    return -1;
  }*/
  if(framenum < 1) {
    std::cerr << "<framenum> must at least be 1" << std::endl;
    return -1;
  }
  tbb::task_scheduler_init init(threadnum);

  // Warn if cfl enabled
  cfl_warn();

  using namespace fluid;

  std::cout << "Loading file \"" << argv[3] << "\"..." << std::endl;
  simulation_istream file(argv[3]);

  float ppm; // ppm is always stored as a float in file.
  unsigned int np;
  file.read_header(ppm,np);

  simulation_type sim(ppm,np);

  sim.read(file);
  std::cout << "Number of cells: " << sim.num_cells() << std::endl;
  std::cout << "Number of particles: " << np << std::endl;
  std::cout << "Particles per meter: " << ppm << std::endl;

  xul::time_meter::optional_meter<xul::time_meter::system_meter<std::chrono::system_clock>> meter;
  meter.start();

  for(int i = 0; i < framenum; ++i) {
    sim.advance_frame();
  }

  meter.stop();

  if(argc > 4) {
    std::cout << "Saving file \"" << argv[4]<< "\"..." << std::endl;
    simulation_ostream file(argv[4]);
    sim.write(file);
  }

  if (meter.is_active()) {
    std::cout << "Simulation time: " << meter.count<std::chrono::microseconds>() << std::endl;
  }

  return 0;
}

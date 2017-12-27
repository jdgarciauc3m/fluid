#ifndef FLUID_SIMULATION_STREAM_H
#define FLUID_SIMULATION_STREAM_H

#include "space_vector.h"
#include <xul/endian/endian_converter.h>
#include <stdexcept>
#include <fstream>

namespace fluid {

class simulation_istream {
public:
  simulation_istream(const std::string & name);
  void read_header(float & ppm, unsigned int & np);

  template <typename F>
  space_vector<F> read_space_vector();

private:
  std::ifstream stream_;
  using endian_type = xul::endian::endian_type;
  xul::endian::static_endian_converter<endian_type::little> endian_;
  constexpr static int INT_SIZE = 4;
  constexpr static int FLOAT_SIZE = 4;
};

class simulation_ostream {
public:
  simulation_ostream(const std::string & name);
  void write_header(float ppm, unsigned int np);
  
  template <typename F>
  void write_space_vector(const space_vector<F> & v);
  
private:
  std::ofstream stream_;
  using endian_type = xul::endian::endian_type;
  xul::endian::static_endian_converter<endian_type::little> endian_;
  constexpr static int INT_SIZE = 4;
  constexpr static int FLOAT_SIZE = 4;
};

simulation_istream::simulation_istream(const std::string & name)
:
stream_(name, std::ios::binary)
{
  if (!stream_) {
    throw std::runtime_error("Error opening input file");
  }
}

void simulation_istream::read_header(float & ppm, unsigned int & np) {
  using namespace xul::endian;

  byte_sequence<FLOAT_SIZE> ppm_seq;
  ppm_seq.read(stream_);
  ppm = endian_.to_host<float>(ppm_seq);

  byte_sequence<INT_SIZE> np_seq;
  np_seq.read(stream_);
  np = endian_.to_host<unsigned int>(np_seq);
}

template <class F>
space_vector<F> simulation_istream::read_space_vector() {
  using namespace xul::endian;
  byte_sequence<FLOAT_SIZE> x, y, z;
  x.read(stream_);
  y.read(stream_);
  z.read(stream_);
  return space_vector<F>{
    endian_.to_host<float>(x),
    endian_.to_host<float>(y),
    endian_.to_host<float>(z)
  }; 
}

simulation_ostream::simulation_ostream(const std::string & name)
:
stream_(name, std::ios::binary)
{
  if (!stream_) {
    throw std::runtime_error("Error opening output file");
  }
}

void simulation_ostream::write_header(float ppm, unsigned int np) {
  static_assert(sizeof(ppm) == FLOAT_SIZE, "Unsupported size for particles per meter");
  static_assert(sizeof(np) == INT_SIZE, "Unsupported size for number of particles");

  using namespace xul::endian;

  endian_.from_host<float>(ppm).write(stream_);
  endian_.from_host<unsigned>(np).write(stream_);
}

template <class F>
void simulation_ostream::write_space_vector(const space_vector<F> & v)
{
  endian_.from_host<float>(v.x()).write(stream_);
  endian_.from_host<float>(v.y()).write(stream_);
  endian_.from_host<float>(v.z()).write(stream_);
}

}

#endif

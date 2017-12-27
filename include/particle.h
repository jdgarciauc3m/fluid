#ifndef FLUID_PARTICLE_H
#define FLUID_PARTICLE_H

#include "params.h"
#include "domain.h"
#include <yapl/cube_index.h>

namespace fluid {

template <typename T>
class particle {
public:

  using index_type = yapl::cube_index;

public:
  particle(const space_vector<T> & p, const space_vector<T> & hv, const space_vector<T> & v);

  particle(const particle & p);
  particle & operator=(const particle & p) = delete;

  particle(particle && p) = delete;
  particle & operator=(particle && p) = delete;

  index_type grid_position(const domain<T> & d) const;

  template <unsigned int I>
  T next_position() const;

  template <int D>
  void process_collision_lower();

  template <int D>
  void process_collision_upper();

  template <int I>
  void reprocess_collision_lower();	

  template <int I>
  void reprocess_collision_upper();

  void advance();

  void increase_densities(particle<T> & p, T hsq);
  void transform_density(T dc, T h6);
  void transfer_acceleration(particle<T> & p, T h, T hsq, T pc, T vc);

  void write(simulation_ostream & os) const;

  template <class OS>
  friend OS & operator<<(OS & os, const particle & p) {
    return os << "P : " << p.position_ << std::endl;
  }

private:

  template <unsigned int D>
  T distance_to_lower_limit(T pos) const;

  template <unsigned int D>
  T distance_to_upper_limit(T pos) const;

  template <unsigned int D>
  void process_collision(T diff);

  template <unsigned int D>
  void increase_acceleration(T diff);


private:
  space_vector<T> position_;
  space_vector<T> hv_;
  space_vector<T> velocity_;
  space_vector<T> acceleration_;
  T density_;
};

template <typename T>
particle<T>::particle(const space_vector<T> & p, const space_vector<T> & hv, const space_vector<T> & v)
:
  position_{p},
  hv_{hv},
  velocity_{v},
  acceleration_{constants::EXTERNAL_ACCELERATION<T>()},
  density_{}
{
}

template <typename T>
particle<T>::particle(const particle & p)
:
particle{p.position_, p.hv_, p.velocity_}
{
}

template <typename T>
yapl::cube_index particle<T>::grid_position(const domain<T> & d) const
{
  return d.grid_position(position_);
}

template <typename T>
template <unsigned int D>
T particle<T>::next_position() const
{
  using namespace constants;
  return position_.template get<D>() + hv_.template get<D>() * TIME_STEP<T>();
}

template <typename T>
template <int I>
void particle<T>::process_collision_lower()
{
  using namespace constants;
  T diff = PARTICLE_SIZE<T>() - distance_to_lower_limit<I>(next_position<I>());
  if (diff > EPSILON<T>()) {
    increase_acceleration<I>(diff);
  }
}

template <typename T>
template <int I>
void particle<T>::process_collision_upper()
{
  using namespace constants;
  T diff = PARTICLE_SIZE<T>() - distance_to_upper_limit<I>(next_position<I>());
  if (diff > EPSILON<T>()) {
    increase_acceleration<I>(-diff);
  }
}

template <typename T>
template <int D>
void particle<T>::reprocess_collision_lower()
{
  using namespace constants;
  T diff = distance_to_lower_limit<D>(position_.template get<D>());
  if (diff < T{}) {
    process_collision<D>(DOMAIN_MIN<T>().template get<D>() - diff);
  }
}

template <typename T>
template <int D>
void particle<T>::reprocess_collision_upper()
{
  using namespace constants;
  T diff = distance_to_upper_limit<D>(position_.template get<D>());
  if (diff < T{}) {
    process_collision<D>(DOMAIN_MAX<T>().template get<D>() + diff);
  }
}

template <typename T>
void particle<T>::advance()
{
  using namespace constants;
  space_vector<T> v_half = hv_ + acceleration_ * TIME_STEP<T>();
  position_ += v_half * TIME_STEP<T>();
  velocity_ = hv_ + v_half;
  velocity_ *= 0.5;
  hv_ = v_half;
}

template <typename T>
void particle<T>::increase_densities(particle<T> & p, T hsq)
{
  T distsq = position_.square_distance(p.position_);
  if (distsq < hsq) {
    T t = hsq - distsq;
    T tc = t * t * t;
    density_ += tc;
    p.density_ += tc;
  }
}

template <typename T>
void particle<T>::transfer_acceleration(particle<T> & p, T h, T hsq, T pc, T vc)
{
  using namespace constants;
  auto disp = position_ - p.position_;
  T distsq = disp.norm();
  if (distsq < hsq) {
    T dist = std::sqrt(std::max(distsq, T(1e-12)));
    T hmr = h - dist;

    auto acc = disp * pc * (hmr * hmr / dist);
    acc *= (density_ + p.density_ - DOUBLE_REST_DENSITY<T>());
    acc += (p.velocity_ - velocity_) * vc * hmr;
    acc /= density_ * p.density_;

    acceleration_ += acc;
    p.acceleration_ -= acc;
  }
}

template <typename T>
void particle<T>::transform_density(T dc, T h6)
{
  density_ += h6;
  density_ *= dc;
}

template <typename T>
void particle<T>::write(simulation_ostream & os) const
{
  os.write_space_vector(position_);
  os.write_space_vector(hv_);
  os.write_space_vector(velocity_);
}

template <typename T>
template <unsigned int D>
T particle<T>::distance_to_lower_limit(T pos) const
{
  using namespace constants;
  return pos - DOMAIN_MIN<T>().template get<D>();
}

template <typename T>
template <unsigned int D>
T particle<T>::distance_to_upper_limit(T pos) const
{
  using namespace constants;
  return DOMAIN_MAX<T>().template get<D>() - pos;
}

template <typename T>
template <unsigned int D>
void particle<T>::process_collision(T diff) 
{
  position_.template get<D>() = diff;
  velocity_.template get<D>() = -velocity_.template get<D>();
  hv_.template get<D>() = -hv_.template get<D>();
}

template <typename T>
template <unsigned int D>
void particle<T>::increase_acceleration(T diff)
{
  using namespace constants;
  acceleration_.template get<D>() += STIFFNESS_COLLISIONS<T>() * diff - DAMPING<T>() * velocity_.template get<D>();
}



}

#endif

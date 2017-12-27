#ifndef FLUID_PARAMS_H
#define FLUID_PARAMS_H

#include <cmath>

namespace fluid {

namespace constants {

template <typename T>
constexpr T PI()
{
  return 3.14159265358979;
}

template <typename T>
constexpr T KERNEL_RADIUS_MULTIPLIER()
{ 
  return 1.695;
}

template <typename T>
constexpr T DOUBLE_REST_DENSITY()
{ 
  return 2000; 
}

template <typename T>
constexpr T STIFFNESS_PRESSURE() 
{ 
  return 3.0; 
}

template <typename T>
constexpr T VISCOSITY() 
{ 
  return 0.4; 
}

template <typename T>
constexpr T PARTICLE_SIZE() 
{ 
  return 0.0002; 
}

template <typename T>
constexpr T EPSILON() 
{ 
  return 1e-10; 
}

template <typename T>
constexpr T TIME_STEP()
{ 
  return 0.001; 
}

template <typename T>
constexpr T STIFFNESS_COLLISIONS()
{ 
  return 30000; 
}

template <typename T>
constexpr T DAMPING() 
{ 
  return 128; 
}

template <typename T>
constexpr space_vector<T> EXTERNAL_ACCELERATION() 
{ 
  return {T{}, T(-9.8), T{}};
}

template <typename T>
constexpr space_vector<T> DOMAIN_MAX()
{
  return {0.065, 0.1, 0.065};
}

template <typename T>
constexpr space_vector<T> DOMAIN_MIN()
{
  return {-0.065, -0.08, -0.065};
}

template <typename T>
constexpr space_vector<T> DOMAIN_RANGE() 
{
  return DOMAIN_MAX<T>() - DOMAIN_MIN<T>();
}


}

template <typename T>
class params {
public:
  params(T ppm);

private:
  static T coeff1(T h) { 
    using namespace constants;
    return 315.0 / (64.0 * PI<T>() * T(std::pow(h,T(9)))); 
  }
  static T coeff2(T h) { 
    using namespace constants;
    return 15.0 / (PI<T>() * std::pow(h,T(6))); 
  }
  static T coeff3(T h) { 
    using namespace constants;
    return 45.0 / (PI<T>() * std::pow(h,T(6))); 
  }
  static T particle_mass(T ppm) { 
    using namespace constants;
    return 0.5 * DOUBLE_REST_DENSITY<T>() / (ppm * ppm * ppm); 
  }

  static T compute_h(T ppm) { 
    using namespace constants;
    return KERNEL_RADIUS_MULTIPLIER<T>() / ppm; 
  }

  static T compute_density_coeff(T ppm, T h) { 
    return particle_mass(ppm) * coeff1(h); 
  }

  static T compute_pressure_coeff(T ppm, T h) { 
    using namespace constants;
    return T(3.0) * coeff2(h) * T(0.5) * STIFFNESS_PRESSURE<T>() * particle_mass(ppm); 
  }

  static T compute_viscosity_coeff(T ppm, T h) { 
    using namespace constants;
    return VISCOSITY<T>() * coeff3(h) * particle_mass(ppm); 
  }

public:
  const T h_;
  const T hsq_;
  const T h6_;
  const T density_coeff_;
  const T pressure_coeff_;
  const T viscosity_coeff_;

};


template <typename T>
params<T>::params(T ppm)
:
h_{compute_h(ppm)},
hsq_{h_*h_},
h6_{hsq_ * hsq_ * hsq_},
density_coeff_{compute_density_coeff(ppm,h_)},
pressure_coeff_{compute_pressure_coeff(ppm,h_)},
viscosity_coeff_{compute_viscosity_coeff(ppm,h_)}
{
}

}
#endif

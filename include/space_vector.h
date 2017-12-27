#ifndef FLUID_SPACE_VECTOR_H
#define FLUID_SPACE_VECTOR_H

#include <yapl/cube_index.h>

template <class T>
class space_vector
{
public:

private:
  T x_, y_, z_;

public:
  T x() const { return x_; }
  T y() const { return y_; }
  T z() const { return z_; }

  constexpr space_vector() noexcept{}
  constexpr space_vector(T _x, T _y, T _z) noexcept : x_(_x), y_(_y), z_(_z) {}

  template <typename U>
  constexpr space_vector(const space_vector<U> & v) : space_vector(v.x(), v.y(), v.z()) {}

  constexpr space_vector(const space_vector & v) noexcept = default;
  space_vector & operator=(const space_vector & v) noexcept = default;

  constexpr space_vector(space_vector && v) noexcept = default;
  space_vector & operator=(space_vector &&) noexcept = default;

  constexpr space_vector(const yapl::cube_index & i) noexcept : space_vector(i.get<0>(), i.get<1>(), i.get<2>()) {}

  constexpr operator std::tuple<T,T,T>() { return std::make_tuple(x_, y_, z_); }

  constexpr bool operator==(const space_vector & v) const { return x_==v.x_ && y_==v.y_ && z_==v.z_; }
  constexpr bool operator>=(const space_vector & v) const { return x_>=v.x_ && y_>=v.y_ && z_>=v.z_; }
  constexpr bool operator>=(T c) const { return x_>=c && y_>=c && z_>=c; }


  template <int I>
  T  & get() { return (I==0)?x_:((I==1)?y_:z_); }

  template <int I>
  T get() const { return (I==0)?x_:((I==1)?y_:z_); }

  T square_distance(space_vector & v) const { return (*this - v).norm(); }
  T norm() const { return x_*x_ + y_*y_ + z_*z_; }

  constexpr T volume() const { return x_ * y_ * z_; }

  void box(const space_vector & min, const space_vector & max) {
    x_ = (x_<min.x_)?min.x_:(x_>max.x_)?max.x_:x_;
    y_ = (y_<min.y_)?min.y_:(y_>max.y_)?max.y_:y_;
    z_ = (z_<min.z_)?min.z_:(z_>max.z_)?max.z_:z_;
  }

  space_vector &  operator += (space_vector const &v) { x_ += v.x_;  y_ += v.y_; z_ += v.z_; return *this; }

  template <typename U>
  space_vector &  operator -= (space_vector<U> const &v) { x_ -= v.get<0>();  y_ -= v.get<1>(); z_ -= v.get<2>(); return *this; }

  space_vector &  operator *= (T s)      { x_ *= s;  y_ *= s; z_ *= s; return *this; }
  space_vector &  operator /= (T s)      { T tmp = 1.f/s; x_ *= tmp;  y_ *= tmp; z_ *= tmp; return *this; }

  space_vector    operator + (space_vector const &v) const    { return space_vector(x_+v.x_, y_+v.y_, z_+v.z_); }
  space_vector    operator + (T const &f) const  { return space_vector(x_+f, y_+f, z_+f); }
  space_vector    operator - () const                 { return space_vector(-x_, -y_, -z_); }
  constexpr space_vector    operator - (space_vector const &v) const    { return space_vector(x_-v.x_, y_-v.y_, z_-v.z_); }
  space_vector    operator * (T s) const         { return space_vector(x_*s, y_*s, z_*s); }
  space_vector    operator / (T s) const         { T tmp = 1.f/s; return space_vector(x_*tmp, y_*tmp, z_*tmp); }

  template <typename U>
  constexpr space_vector operator/(const space_vector<U> & v) { return space_vector<T>{x_/v.x(), y_/v.y(), z_/v.z()}; }

  T  operator * (space_vector const &v) const    { return x_*v.x_ + y_*v.y_ + z_*v.z_; }

  template <class O>
  friend O & operator<<(O & o, const space_vector & v) { return o << "( " << v.x_ << " , " << v.y_ << " , " << v.z_ << " )"; }
};

#endif

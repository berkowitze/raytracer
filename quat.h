#ifndef QUAT_H
#define QUAT_H
#include "vec3.h"

class quat
{
public:
  double w;
  vec3 v;

  quat(double x, double y, double z, double w) : w(w), v(x, y, z) {}
  quat(vec3 v, double w) : w(w), v(v) {}
  quat(double w, vec3 v) : w(w), v(v) {}
  ~quat() = default;

  double norm() const;
  quat normalized() const;
  void normalize();
  void convertToUnitNormQuaternion();
  quat conjugate() const { return quat(-v, w); }
  quat inverse() const;
};

inline quat operator+(const quat &A, const quat &B)
{
  return quat(A.v + B.v, A.w + B.w);
}
inline quat operator-(const quat &A, const quat &B)
{
  return quat(A.v - B.v, A.w - B.w);
}
inline void operator+=(quat &A, const quat &B)
{
  A.v += B.v;
  A.w += B.w;
}
inline void operator-=(quat &A, const quat &B)
{
  A.v -= B.v;
  A.w -= B.w;
}

inline quat operator*(const quat &A, const quat &B)
{
  double scalar = A.w * B.w - dot(A.v, B.v);
  vec3 vector = A.w * B.v + B.w * A.v + cross(A.v, B.v);
  return quat(vector, scalar);
}
inline void operator*=(quat &A, const quat &B)
{
  quat result = A * B;
  A.v = result.v;
  A.w = result.w;
}
inline quat operator*(const quat &A, double t)
{
  return quat(A.v * t, A.w * t);
}
inline vec3 operator*(const quat &A, const vec3 &B)
{
  quat p(0, B);
  quat q = A * p * A.inverse();
  return q.v;
}
inline void operator*=(quat &A, double t)
{
  A.v = A.v * t;
  A.w = A.w * t;
}

inline double quat::norm() const
{
  return std::sqrt(w * w + v.length_squared());
}

inline quat quat::normalized() const
{
  double inv_norm = 1 / this->norm();
  return quat(v * inv_norm, w * inv_norm);
}
inline void quat::normalize()
{
  double inv_norm = 1 / this->norm();
  v *= inv_norm;
  w *= inv_norm;
}
inline quat quat::inverse() const
{
  double norm_squared = norm() * norm();
  return quat(-v / norm_squared, w / norm_squared);
}
void quat::convertToUnitNormQuaternion()
{
  double angle = w;
  v.normalize();
  w = cos(angle * 0.5);
  v *= sin(angle * 0.5);
}

vec3 rotate_about_axis(double angle, vec3 &axis, vec3 &point)
{
  quat p(0, point);
  axis.normalize();
  quat q(angle, axis);
  q.convertToUnitNormQuaternion();
  quat q_inv = q.inverse();
  quat rotated = q * p * q_inv;
  return rotated.v;
}

#endif

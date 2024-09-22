#ifndef ONB_H
#define ONB_H

// class for orthonormal bases
class onb
{
public:
  onb(const vec3 &n)
  {
    axes[2] = unit_vector(n);
    // take care not to generate a vector parallel to n
    vec3 a = (std::fabs(axes[2].x()) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);
    axes[1] = unit_vector(cross(axes[2], a));
    axes[0] = cross(axes[2], axes[1]);
  }

  const vec3 &u() const
  {
    return axes[0];
  }

  const vec3 &v() const
  {
    return axes[1];
  }

  const vec3 &w() const
  {
    return axes[2];
  }

  vec3 transform(const vec3 &p) const
  {
    return (p[0] * axes[0]) + (p[1] * axes[1]) + (p[2] * axes[2]);
  }

private:
  vec3 axes[3];
};

#endif
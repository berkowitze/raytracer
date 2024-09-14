#ifndef AABB_H
#define AABB_H

// Axis-aligned bounding box
class aabb
{
public:
  interval x, y, z;

  aabb() {} // Default AABB is empty
  aabb(const interval &x, const interval &y, const interval &z) : x(x), y(y), z(z) {}
  aabb(const aabb &bbox1, const aabb &bbox2)
  {
    x = interval(bbox1.x, bbox2.x);
    y = interval(bbox1.y, bbox2.y);
    z = interval(bbox1.z, bbox2.z);
  }
  aabb(const point3 &a, const point3 &b)
  {
    // construct bounding box based on box swept by two points
    x = a[0] < b[0] ? interval(a[0], b[0]) : interval(b[0], a[0]);
    y = a[1] < b[1] ? interval(a[1], b[1]) : interval(b[1], a[1]);
    z = a[2] < b[2] ? interval(a[2], b[2]) : interval(b[2], a[2]);
  }

  const interval &axis_interval(int n) const
  {
    // return axis associated with axis number (0=x, 1=y, 2=z)
    if (n == 0)
      return x;
    if (n == 1)
      return y;
    return z;
  }

  // determine if ray r intersects this aabb with a time in the ray_t interval
  // there needs to be a common overlap in the intersection times for all 3 axes for
  // this to return true. if the x and y coordinates of the ray are in the box at the
  // same time, but the z coordinate is not, we need to throw it out.
  // that means we need to track the largest interval of intersection times that applies
  // as we iterate through the axes, which shrinks with each iteration. This starts off
  // as ray_t
  bool hit(const ray &r, interval ray_t) const
  {
    const point3 &ray_origin = r.origin();
    const vec3 &ray_dir = r.direction();

    for (int axis = 0; axis <= 2; axis++)
    {
      const interval &axi = axis_interval(axis);

      const double d_inverse = 1.0 / ray_dir[axis];
      // t0 and t1 are the solutions to P(t) = Q + td where P is the position along a ray, t is
      // "time", Q is the start position of the ray, and d is the direction of the ray
      const double t0 = (axi.min - ray_origin[axis]) * d_inverse;
      const double t1 = (axi.max - ray_origin[axis]) * d_inverse;
      // Rebound ray_t interval to be the tightest
      if (t0 < t1)
      {
        ray_t.min = std::max(ray_t.min, t0);
        ray_t.max = std::min(ray_t.max, t1);
      }
      else
      {
        ray_t.min = std::max(ray_t.min, t1);
        ray_t.max = std::min(ray_t.max, t0);
      }

      if (ray_t.max <= ray_t.min)
        return false;
    }

    return true;
  }

  int largest_axis() const
  {
    if (x.size() > y.size() && x.size() > z.size())
    {
      return 0;
    }
    if (y.size() > x.size() && y.size() > z.size())
    {
      return 1;
    }
    return 2;
  }

  static const aabb empty, universe;
};

const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);

#endif
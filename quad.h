#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"

class quad : public hittable
{
public:
  quad(
      const point3 &Q,
      const vec3 &u,
      const vec3 &v,
      shared_ptr<material> material)
      : Q(Q), u(u), v(v), material(material)
  {
    vec3 n = cross(u, v);
    w = n / dot(n, n);
    normal = unit_vector(n);
    D = dot(normal, Q);
    set_bounding_box();
  }

  virtual void set_bounding_box()
  {
    // Bottom left to top right (assuming positive octant)
    aabb bbox_diagonal1 = aabb(Q, Q + u + v);
    // Top left to bottom right (same assumption)
    aabb bbox_diagonal2 = aabb(Q + u, Q + v);
    bbox = aabb(bbox_diagonal1, bbox_diagonal2);
  }

  aabb bounding_box() const override
  {
    return bbox;
  }

  bool hit(const ray &r, interval ray_t, hit_record &hit_record) const override
  {
    double denominator = dot(normal, r.direction());

    // ray is parallel to plane
    if (std::fabs(denominator) < 1e-8)
      return false;

    double t = (D - dot(normal, r.origin())) / denominator;
    if (!ray_t.contains(t))
    {
      return false;
    }
    point3 intersection = r.at(t);

    vec3 p = intersection - Q;          // for plane coordinate transformations
    double alpha = dot(w, cross(p, v)); // basis constants for plane coordinates
    double beta = dot(w, cross(u, p));
    if (alpha > 1 || beta > 1 || alpha < 0 || beta < 0)
    {
      return false;
    }
    hit_record.u = alpha;
    hit_record.v = beta;
    hit_record.p = intersection;
    hit_record.t = t;
    hit_record.set_face_normal(r, normal);
    hit_record.mat = material;
    return true;
  }

private:
  point3 Q;
  vec3 u;
  vec3 v;

  vec3 normal;
  double D;
  vec3 w; // for plane coordinate transformations

  shared_ptr<material> material;

  aabb bbox;
};

inline shared_ptr<hittable_list> box(const point3 &a, const point3 &b, shared_ptr<material> material)
{
  auto sides = make_shared<hittable_list>();

  point3 min = point3(std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()), std::fmin(a.z(), b.z()));
  point3 max = point3(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()), std::fmax(a.z(), b.z()));

  vec3 dx = vec3(max.x() - min.x(), 0, 0);
  vec3 dy = vec3(max.y() - min.y(), 0, 0);
  vec3 dz = vec3(max.z() - min.z(), 0, 0);

  // these 6 lines copied from tutorial directly
  sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy, material));  // front
  sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, material)); // right
  sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, material)); // back
  sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy, material));  // left
  sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz, material)); // top
  sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz, material));  // bottom
  return sides;
}

#endif
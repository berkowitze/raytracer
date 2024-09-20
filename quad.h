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

#endif
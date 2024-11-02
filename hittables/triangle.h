#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "../hittable.h"

class tri : public hittable
{
public:
  tri(
      const point3 &v1,
      const point3 &v2,
      const point3 &v3,
      const vec3 _normal,
      const point3 &uv1,
      const point3 &uv2,
      const point3 &uv3,
      shared_ptr<material> material)
      : v1(v1), v2(v2), v3(v3), uv1(uv1), uv2(uv2), uv3(uv3), material(material)
  {
    u = v2 - v1;
    v = v3 - v1;
    vec3 n = cross(u, v);
    w = n / dot(n, n);
    normal = unit_vector(n);
    D = dot(normal, v1);
    set_bounding_box();
  }

  virtual void set_bounding_box()
  {
    // Bottom left to top right (assuming positive octant)
    aabb bbox_diagonal1 = aabb(v1, v1 + u + v);
    // Top left to bottom right (same assumption)
    aabb bbox_diagonal2 = aabb(v1 + u, v1 + v);
    bbox = aabb(bbox_diagonal1, bbox_diagonal2);
  }

  aabb bounding_box() const override
  {
    return bbox;
  }

  bool hit(const ray &r, interval ray_t, hit_record &hit_record) const override
  {
    // Möller-Trumbore algorithm
    vec3 h = cross(r.direction(), v);
    float a = dot(u, h);
    if (a > -0.0001f && a < 0.0001f)
      return false;

    float f = 1 / a;
    vec3 s = r.origin() - v1;

    float ud = f * dot(s, h);
    if (ud < 0 || ud > 1)
      return false;
    vec3 q = cross(s, u);
    float vd = f * dot(r.direction(), q);
    if (vd < 0 || ud + vd > 1)
      return false;
    float t = f * dot(v, q);
    if (!ray_t.contains(t))
      return false;

    point3 intersection = r.at(t);
    float wd = 1 - ud - vd;
    vec3 uv = wd * uv1 + ud * uv2 + vd * uv3;

    hit_record.u = uv.x();
    hit_record.v = uv.y();
    hit_record.p = intersection;
    hit_record.t = t;
    hit_record.set_face_normal(r, normal);
    hit_record.mat = material;
    return true;
  }

private:
  point3 v1;
  point3 v2;
  point3 v3;

  vec3 u;
  vec3 v;

  vec3 uv1;
  vec3 uv2;
  vec3 uv3;

  vec3 normal;
  double D;
  vec3 w; // for plane coordinate transformations

  shared_ptr<material> material;

  aabb bbox;
};

#endif
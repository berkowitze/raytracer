#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "../hittable.h"

class constant_medium : public hittable
{
public:
  constant_medium(
      shared_ptr<hittable> boundary,
      double density,
      shared_ptr<texture> texture) : boundary(boundary), neg_inv_density(-1.0 / density), phase_function(make_shared<isotropic>(texture)) {}

  constant_medium(shared_ptr<hittable> boundary,
                  double density,
                  color albedo) : boundary(boundary), neg_inv_density(-1.0 / density), phase_function(make_shared<isotropic>(albedo)) {}

  bool hit(const ray &r, interval ray_t, hit_record &rec) const override
  {
    hit_record rec_1;
    hit_record rec_2;

    // does the ray hit one side of the boundary (even if time is negative)
    if (!boundary->hit(r, interval::universe, rec_1))
    {
      return false;
    }
    // does the ray then hit the other side of the boundary after hitting the first side, even if time is still negative
    if (!boundary->hit(r, interval(rec_1.t + 0.0001, infinity), rec_2))
    {
      return false;
    }

    // If the rays intersect the boundary outside of ray_t, that means
    // they travel outside of the boundary for some time - so for the rest
    // of this calculation, only pay attention to the time they are within
    // the boundary
    if (rec_1.t < ray_t.min)
      rec_1.t = ray_t.min;
    if (rec_2.t > ray_t.max)
      rec_2.t = ray_t.max;

    if (rec_1.t == rec_2.t)
    {
      return false;
    }

    // Don't pay attention to time the ray was traveling inside the boundary, but
    // earlier than t=0
    if (rec_1.t < 0)
    {
      rec_1.t = 0;
    }

    double ray_length = r.direction().length();
    double distance_within_boundary = (rec_2.t - rec_1.t) * ray_length;
    double hit_distance = neg_inv_density * std::log(random_double());
    if (hit_distance > distance_within_boundary)
    {
      return false;
    }
    // move t into the boundary by hit_distance amount
    rec.t = rec_1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    rec.normal = vec3(1, 0, 0); // doesn't matter
    rec.front_face = true;      // doesn't matter
    rec.mat = phase_function;

    return true;
  }

  aabb bounding_box() const override
  {
    return boundary->bounding_box();
  }

private:
  shared_ptr<hittable> boundary;
  double neg_inv_density;
  shared_ptr<material> phase_function;
};

#endif
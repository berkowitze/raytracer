#ifndef ROTATE_H
#define ROTATE_H

#include "../hittable.h"

class rotate : public hittable
{
public:
  rotate(shared_ptr<hittable> object, int axis, double rotation_degrees) : axis(axis), object(object)
  {
    cos_theta = cos(degrees_to_radians(rotation_degrees));
    sin_theta = sin(degrees_to_radians(rotation_degrees));
    bbox = object->bounding_box();

    point3 min = point3(infinity);
    point3 max = point3(-infinity);

    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
      {
        for (int k = 0; k < 2; k++)
        {
          // get coordinates of bbox vertex currently getting transformed
          double x = i * bbox.x.max + (1 - i) * bbox.x.min;
          double y = j * bbox.y.max + (1 - j) * bbox.y.min;
          double z = k * bbox.z.max + (1 - k) * bbox.z.min;

          vec3 rotated_vertex = get_negative_rotated_vector(vec3(x, y, z));
          // Update min and max to tightly constrain the rotated bounding box
          for (int c = 0; c < 3; c++)
          {
            min[c] = std::fmin(min[c], rotated_vertex[c]);
            max[c] = std::fmax(max[c], rotated_vertex[c]);
          }
        }
      }
    }
    bbox = aabb(min, max);
  }

  aabb bounding_box() const override
  {
    return bbox;
  }

  bool hit(const ray &r, interval ray_t, hit_record &hit_record) const override
  {
    vec3 origin = get_rotated_vector(r.origin());
    vec3 direction = get_rotated_vector(r.direction());

    ray rotated_ray = ray(origin, direction);

    if (!object->hit(rotated_ray, ray_t, hit_record))
    {
      return false;
    }
    hit_record.p = get_negative_rotated_vector(hit_record.p);
    hit_record.normal = get_negative_rotated_vector(hit_record.normal);
    return true;
  }

private:
  int axis; // 0 = x, 1 = y, 2 = z
  double cos_theta;
  double sin_theta;

  shared_ptr<hittable> object;
  aabb bbox;

  vec3 get_rotated_vector(const vec3 &p) const
  {
    // if (axis == 0)
    // {
    //   double yp = cos_theta * p.y() - sin_theta * p.z();
    //   double zp = sin_theta * p.y() + cos_theta * p.z();
    //   return vec3(p.x(), yp, zp);
    // }
    // else if (axis == 1)
    // {
    double xp = (cos_theta * p.x()) - (sin_theta * p.z());
    double zp = (sin_theta * p.x()) + (cos_theta * p.z());
    return vec3(xp, p.y(), zp);
    // }
    // // not implemented
    // return p;
  }

  vec3 get_negative_rotated_vector(const vec3 &p) const
  {
    // if (axis == 0)
    // {
    //   double y = cos_theta * p.y() + sin_theta * p.z();
    //   double z = -sin_theta * p.y() + cos_theta * p.z();
    //   return vec3(p.x(), y, z);
    // }
    // else if (axis == 1)
    // {
    double x = (cos_theta * p.x()) + (sin_theta * p.z());
    double z = (-sin_theta * p.x()) + (cos_theta * p.z());
    return vec3(x, p.y(), z);
    // }
    // return p;
  }
};

#endif

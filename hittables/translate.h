#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "../hittable.h"

class translate : public hittable
{
public:
  translate(shared_ptr<hittable> object, const vec3 &offset) : object(object), offset(offset)
  {
    bbox = object->bounding_box() + offset;
  }

  aabb bounding_box() const override
  {
    return bbox;
  }

  bool hit(const ray &r, interval ray_t, hit_record &hit_record) const override
  {
    ray offset_ray = ray(r.origin() - offset, r.direction(), r.time());
    if (object->hit(offset_ray, ray_t, hit_record))
    {
      hit_record.p = hit_record.p + offset;
      return true;
    }
    return false;
  }

private:
  shared_ptr<hittable> object;
  vec3 offset;
  aabb bbox;
};

#endif
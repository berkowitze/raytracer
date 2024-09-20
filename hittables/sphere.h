#ifndef SPHERE_H
#define SPHERE_H

#include "../hittable.h"

class sphere : public hittable
{
public:
    // Stationary
    sphere(
        const point3 &center,
        double radius,
        shared_ptr<material> mat) : center(ray(center, vec3())), radius(radius), mat(mat)
    {
        bbox = aabb(center - vec3(radius), center + vec3(radius));
    }

    // Moving
    sphere(
        const point3 &center1,
        const point3 &center2,
        double radius,
        shared_ptr<material> mat) : center(center1, center2 - center1), radius(radius), mat(mat)
    {
        vec3 rvec = vec3(radius);
        aabb center1_bbox = aabb(center1 - rvec, center1 + rvec);
        aabb center2_bbox = aabb(center2 - rvec, center2 + rvec);
        bbox = aabb(center1_bbox, center2_bbox);
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        point3 current_center = center.at(r.time());
        vec3 oc = current_center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - (radius * radius);
        auto discriminant = h * h - a * c;
        if (discriminant < 0)
        {
            return false;
        }
        auto root = (h - std::sqrt(discriminant)) / a;
        if (!ray_t.surrounds(root))
        {
            root = (h + std::sqrt(discriminant)) / a;

            if (!ray_t.surrounds(root))
            {
                return false;
            }
        }
        rec.t = root;
        rec.p = r.at(rec.t);
        rec.mat = mat;
        vec3 outward_normal = (rec.p - current_center) / radius;
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.set_face_normal(r, outward_normal);
        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

private:
    ray center;
    double radius;
    shared_ptr<material> mat;
    aabb bbox;

    // p should be on a point on the surface of a unit sphere
    static void get_sphere_uv(const point3 &p, double &u, double &v)
    {
        double theta = std::acos(-p.y());
        double phi = std::atan2(-p.z(), p.x()) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }
};

#endif
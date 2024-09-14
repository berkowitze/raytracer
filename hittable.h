#ifndef HITTABLE_H
#define HITTABLE_H

class material;

class hit_record
{
public:
	point3 p;									// location of hit
	vec3 normal;							// normal of surface at hit
	double t;									// "time" along ray that the hit occurred
	bool front_face;					// whether the ray hit from the front side or back side of the face
	shared_ptr<material> mat; // material at the intersection

	void set_face_normal(const ray &r, const vec3 &outward_normal)
	{
		// Set normal vector for hit_record
		// outward_normal must be normalized

		front_face = dot(r.direction(), outward_normal) < 0.0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable
{
public:
	virtual ~hittable() = default;

	virtual bool hit(const ray &r, interval ray_t, hit_record &rec) const = 0;

	virtual aabb bounding_box() const = 0;
};

#endif
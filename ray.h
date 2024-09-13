#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray
{
public:
	ray() {}
	ray(const point3 &origin, const vec3 &direction, double time) : orig(origin), dir(direction), _time(time) {}
	ray(const point3 &origin, const vec3 &direction) : orig(origin), dir(direction) {}

	const point3 &origin() const { return orig; }
	const vec3 &direction() const { return dir; }
	double time() const { return _time; }

	point3 at(double t) const
	{
		return orig + t * dir;
	}

private:
	double _time = 0;
	point3 orig;
	vec3 dir;
};

#endif

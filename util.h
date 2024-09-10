#ifndef UTIL_H
#define UTIL_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <cstdlib>

using std::make_shared;
using std::shared_ptr;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees)
{
	return degrees * pi / 180.0;
}

inline double random_double()
{
	// Double from 0 to 1
	return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max)
{
	// Double from min to max
	return min + (max - min) * random_double();
}

inline double random_double(double max)
{
	// Double from 0 to max
	return max * random_double();
}

#include "interval.h"
#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "camera.h"
#include "material.h"

#endif
#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

using color = vec3;

inline double linear_to_gamma(double linear_component)
{
	if (linear_component > 0)
	{
		return std::sqrt(linear_component);
	}
	else
	{
		return 0;
	}
}

void write_color(std::ostream &out, const color &pixel_color)
{
	auto r = linear_to_gamma(pixel_color.x());
	auto g = linear_to_gamma(pixel_color.y());
	auto b = linear_to_gamma(pixel_color.z());

	if (r != r)
		r = 0.0;
	if (g != g)
		g = 0.0;
	if (b != b)
		b = 0.0;

	static const interval intensity(0.000, 0.999);

	int rbyte = int(256 * intensity.clamp(r));
	int gbyte = int(256 * intensity.clamp(g));
	int bbyte = int(256 * intensity.clamp(b));

	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
	// Flush
	out << std::flush;
}

#endif
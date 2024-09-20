#ifndef INTERVAL_H
#define INTERVAL_H
#include <algorithm>

class interval
{
public:
	double min, max;
	// use an empty interval by default
	interval() : min(infinity), max(-infinity) {}
	interval(double min, double max) : min(min), max(max) {}
	interval(const interval &i1, const interval &i2)
	{
		min = std::min(i1.min, i2.min);
		max = std::max(i1.max, i2.max);
	}

	double size() const
	{
		return max - min;
	}

	bool contains(double x) const
	{
		return x >= min && x <= max;
	}

	bool surrounds(double x) const
	{
		return x > min && x < max;
	}

	double clamp(double x) const
	{
		if (x < min)
			return min;
		if (x > max)
			return max;
		return x;
	}

	interval expanded_by(double delta) const
	{
		auto padding = delta / 2;
		return interval(min - padding, max + padding);
	}

	static const interval empty, universe;
};

const interval interval::empty = interval(infinity, -infinity);
const interval interval::universe = interval(-infinity, infinity);
interval operator+(const interval &i, double offset)
{
	return interval(i.min + offset, i.max + offset);
}

#endif
#pragma once

#include <iostream>
#include <limits>
#include <memory>
#include <numbers>
#include <random>
#include "Vectors.h"

inline double DegreesToRadians(double degrees) {
	return degrees * std::numbers::pi / 180.0;
}

const double infinity = std::numeric_limits<double>::infinity();

inline double RandomDouble() 
{
	static std::mt19937 gen(std::random_device{}());
	static std::uniform_real_distribution<double> DoubleDist(0.0, 1.0);
	return DoubleDist(gen);
}

inline double RandomDouble(double min, double max) 
{
	static std::mt19937 gen(std::random_device{}());
	std::uniform_real_distribution<double> DoubleDist(min, max);
	return DoubleDist(gen);
}

inline double RandomInt(int min, int max) 
{
	static std::mt19937 gen(std::random_device{}());
	std::uniform_int_distribution<int> DoubleDist(min, max);
	return DoubleDist(gen);
}

static vec3 RandomVec3() {
	return vec3(RandomDouble(), RandomDouble(), RandomDouble());
}

static vec3 RandomVec3(double min, double max) {
	return vec3(RandomDouble(min, max), RandomDouble(min, max), RandomDouble(min, max));
}

inline vec3 RandomUnitVec3()
{
	while (true)
	{
		auto v = RandomVec3(-1.0, 1.0);
		auto lensq = v.x * v.x + v.y * v.y + v.z * v.z;
		if (1e-160 < lensq && lensq <= 1)
			return v / std::sqrt(lensq);
	}
}

inline vec3 RandomUnitVec3OnSphere(const vec3 Normal)
{
	vec3 OnUnitSphere = RandomUnitVec3();
	if (vec3::Vec3Dot(OnUnitSphere, Normal) > 0.0f)
		return OnUnitSphere;
	else
		return OnUnitSphere * (-1.0); // we are doing this bcoz a ray cannot pass thorugh a matte solid object, refer to ms paint visualization
}
inline vec3 RandomUnitVec3OnDisk()
{
	while (true)
	{
		auto v = vec3(RandomDouble(-1, 1), RandomDouble(-1, 1), 0);
		auto lensq = v.x * v.x + v.y * v.y + v.z * v.z;
		if (lensq < 1)
		{
			return v;
		}
	}
}


class Interval
{
public:
	Interval()
		: min(-infinity), max(infinity)
	{
	}

	Interval(double min, double max)
		: min(min), max(max)
	{
	}

	Interval(const Interval i1, const Interval i2)
		: min(std::min(i1.min, i2.min)), max(std::max(i1.max, i2.max))
	{
	}

	bool surrounds(double x) const {
		return min < x && x < max;
	}

	double clamp(double val) const
	{
		return val < min ? min : val > max ? max : val;
	}

	const double& GetMin() const 
	{ 
		return min; 
	}
	const double& GetMax() const
	{
		return max; 
	}
	const void SetMin(double newMin)
	{
		min = newMin;
		
	}
	const void SetMax(double newMax)
	{
		max = newMax;
		
	}
	Interval expand(const double delta) const
	{
		auto padding = delta / 2.0;
		return Interval(min - padding, max + padding);
	}

private:
	double min, max;
};
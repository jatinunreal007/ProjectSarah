#pragma once

#include"Vectors.h"

class Ray
{
public:
	Ray() {}

	Ray(const vec3 origin, const vec3 direction, double time)
		: origin(origin), direction(direction), tm(time) 
	{
	}

	Ray(const vec3 origin, const vec3 direction)
		: origin(origin), direction(direction), tm(0)
	{
	}

	vec3 GetOrigin() const
	{
		return origin; 
	}
	vec3 GetDirection() const
	{ 
		return direction;
	}
	double GetTime() const
	{
		return tm;
	}

	vec3 at(double t) const
	{
		return origin + direction * t;
	}

private:
	vec3 origin;
	vec3 direction;
	double tm = 0.0;
};
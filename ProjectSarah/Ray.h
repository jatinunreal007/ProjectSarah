#pragma once

#include"Vectors.h"

class Ray
{
public:
	Ray(vec3 origin, vec3 direction)
		: origin(origin), direction(direction) {
	}
	vec3 GetOrigin() const
	{
		return origin; 
	}
	vec3 GetDirection() const
	{ 
		return direction;
	}
private:
	vec3 origin;
	vec3 direction;
};
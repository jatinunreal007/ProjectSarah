#pragma once
#include "Vectors.h"
#include "Hittables.h"

class Light
{
public:
	Light(vec3 Direction, float intensity)
		: direction(Direction), intensity(intensity)
	{
	}
	vec3 PLightGetDirection() const
	{
		return vec3::Vec3Normalize(direction);
	}
	float PLightGetIntensity() const
	{
		return intensity;
	}
private:
	vec3 direction;
	float intensity;
};





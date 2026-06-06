#pragma once
#include "Vectors.h"
#include "Hittables.h"

class PointLight
{
public:
	PointLight(vec3 Direction, float intensity)
		: direction(Direction), intensity(intensity)
	{
	}
	vec3 PLightGetDirection()
	{
		return vec3::Vec3Normalize(direction);
	}
	float PLightGetIntensity()
	{
		return intensity;
	}
private:
	vec3 direction;
	float intensity;
};

PointLight pl1(vec3(0.5f, -0.5f, -0.02f), 2.0f);



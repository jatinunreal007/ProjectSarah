#pragma once

#include "Color.h"

class texture
{
public:
	virtual ~texture() = default;

	virtual Color value(double u, double v, const vec3& point) const = 0;
};

class SolidColor : public texture
{
public:
	SolidColor(const Color& albedo)
		:albedo(albedo)
	{
	}

	SolidColor(double r, double g, double b)
		:albedo(Color(r, g, b))
	{
	}

	Color value(double u, double v, const vec3& point) const override
	{
		return albedo;
	}

private:
	Color albedo;
};


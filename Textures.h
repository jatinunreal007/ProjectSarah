#pragma once

#include "Color.h"
#include "Vectors.h"

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

class CheckerTexture : public texture
{
public:
	CheckerTexture(double scale, std::shared_ptr<texture> even, std::shared_ptr<texture> odd)
		:t_inv_scale(1.0/scale), even(even), odd(odd)
	{
	}

	CheckerTexture(double scale, const Color& c1, const Color& c2)
		:CheckerTexture(scale, std::make_shared<SolidColor>(c1), std::make_shared<SolidColor>(c2))
	{
	}

	Color value(double u, double v, const vec3& point) const override
	{
		int xInt = int(std::floor(t_inv_scale * point.x));
		int yInt = int(std::floor(t_inv_scale * point.y));
		int zInt = int(std::floor(t_inv_scale * point.z));

		bool is_even = (xInt + yInt + zInt) % 2 == 0;

		return is_even ? even->value(u, v, point) : odd->value(u, v, point);
	}

private:
	double t_inv_scale = 1;
	std::shared_ptr<texture> even, odd;
};
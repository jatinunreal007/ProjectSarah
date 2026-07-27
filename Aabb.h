#pragma once

#include "Utilities.h"
#include "Vectors.h"
#include "Ray.h"

class AABB
{
public:
	Interval x, y, z;

	AABB() 
	{
	}

	AABB(const Interval& ix, const Interval& iy, const Interval& iz)
		: x(ix), y(iy), z(iz)
	{
	}

	AABB(const vec3& a, const vec3& b)
	{
		x = Interval(std::fmin(a.x, b.x), std::fmax(a.x, b.x));
		y = Interval(std::fmin(a.y, b.y), std::fmax(a.y, b.y));
		z = Interval(std::fmin(a.z, b.z), std::fmax(a.z, b.z));
	}

	const Interval& AxisInterval(int axis) const
	{
		switch (axis)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			throw std::out_of_range("Invalid axis index");
		}
	}
	bool Hit(const Ray& r, Interval& ray_t) const
	{
		const vec3 RayOrigin = r.GetOrigin();
		const vec3 RayDirection = r.GetDirection();

		for (int axis = 0; axis < 3; axis++)
		{
			const Interval& ax = AxisInterval(axis);
			const double InvAxisDir = 1.0 /RayDirection[axis];

			auto t0 = (ax.GetMin() - RayOrigin[axis]) * InvAxisDir;
			auto t1 = (ax.GetMax() - RayOrigin[axis]) * InvAxisDir;

			if (t0 < t1)
			{
				if (t0 > ray_t.GetMin())
					ray_t.SetMin(t0);
				if (t1 < ray_t.GetMax())
					ray_t.SetMax(t1);
			}
			else
			{
				if (t1 > ray_t.GetMin())
					ray_t.SetMin(t1);
				if (t0 < ray_t.GetMax())
					ray_t.SetMax(t0);
			}
			if(ray_t.GetMin() > ray_t.GetMax())
				return false;

		}
		return true;
	}
};
#pragma once
#include "Vectors.h"
#include "Ray.h"
#include "Lightings.h"

//Sphere--->
class Sphere
{
public:
	Sphere(float x, float y, float z, float radius)
		: x(x), y(y), z(z), radius(radius), tOut(0.0f)
	{
	}
	bool HitSphere(const vec3& centre, float radius, const Ray& r)
	{
		vec3 OC = r.GetOrigin() - centre;
		float a = vec3::Vec3Dot(r.GetDirection(), r.GetDirection());
		float b = -2.0f * vec3::Vec3Dot(OC, r.GetDirection());
		float c = vec3::Vec3Dot(OC, OC) - radius * radius;
		float discriminant = (b * b) - (4 * a * c);

		if (discriminant < 0) {
			return false;
		}

	    this->tOut = (-b - std::sqrt(discriminant)) / (2.0f * a);
		
		return true;
	}
	vec3 SphereGetNormal(const Ray& r)
	{
		vec3 pointHit = r.GetOrigin() + r.GetDirection() * tOut;
		vec3 normal = pointHit - vec3(x, y, z);
		return vec3(normal / radius);
	}

	vec3 SphereGetCentre()
	{
		return vec3(x, y, z);
	}
	float SphereGetRadius()
	{
		return radius;
	}

private:
	float x, y, z, radius, tOut;
};

//Objects--->
Sphere s1(0.0f, 0.0f, 2.0f, 0.5f);
//Sphere s2(0.5f, 0.0f, 2.0f, 0.5f);
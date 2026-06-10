#pragma once
#include "Vectors.h"
#include "Ray.h"
#include "Lightings.h"

//Sphere--->
class HitRecord
{
public:
	vec3 point;
	vec3 normal;
	double t;
};

class Hittable
{
public:
	virtual ~Hittable() = default;
	virtual bool Hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const = 0;

};

class Sphere : public Hittable
{
public:
	Sphere(vec3 centre, float radius)
		: centre(centre), radius(radius) {}
	
	bool Hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const override
	{
		vec3 OC = r.GetOrigin() - centre;
		float a = vec3::Vec3Dot(r.GetDirection(), r.GetDirection());
		float b = -2.0f * vec3::Vec3Dot(OC, r.GetDirection());
		float c = vec3::Vec3Dot(OC, OC) - radius * radius;
		float discriminant = (b * b) - (4 * a * c);
		auto sqrtDiscriminant = std::sqrt(discriminant);
		if (discriminant < 0) 
		{
			return false;
		}
		auto root = (-b - sqrtDiscriminant) / (2.0f * a);
		if (root < tMin || root > tMax)
		{
			root = (-b + sqrtDiscriminant) / (2.0f * a);

			if (root < tMin || root > tMax)
				return false;
		}
		rec.t = root;
		rec.point = r.GetOrigin() + r.GetDirection() * rec.t;
		rec.normal = (rec.point - centre) / radius;

		return true;
	}

	vec3 SphereGetCentre() const
	{
		return centre;
	}
	float SphereGetRadius() const
	{
		return radius;
	}

private:
	vec3 centre;
	float radius;
};


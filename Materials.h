#pragma once
#include "Hittables.h"
#include "Color.h"


class materials
{
public:
	virtual ~materials() = default;
	virtual bool scatter(const Ray& r, const HitRecord& rec, vec3& attenuation, Ray& scattered) const
	{
		return false;
	}

};

class lambertian : public materials
{
public:
	lambertian(const Color& albedo)
		: albedo(albedo) {}


	bool scatter(const Ray& r, const HitRecord& rec, vec3& attenuation, Ray& scattered) const override
	{
		vec3 scatterDirection = rec.normal + RandomUnitVec3();

		if (scatterDirection.Vec3NearZero())
			scatterDirection = rec.normal;

		scattered = Ray(rec.point, scatterDirection);
		attenuation = albedo;
		return true;
	}
private:
	Color albedo;
};
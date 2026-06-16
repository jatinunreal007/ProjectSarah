#pragma once
#include "Hittables.h"
#include "Vectors.h"

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
	lambertian(const vec3& albedo)
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
	vec3 albedo;
};

class metal : public materials
{
public: 
	metal(const vec3 albedo)
		: albedo(albedo){}

	bool scatter(const Ray& rIncident, const HitRecord& rec, vec3& attenuation, Ray& scattered) const override
	{
		vec3 refl = vec3().Reflect(rIncident.GetDirection(), rec.normal);
		scattered = Ray(rec.point, refl);
		attenuation = albedo;
		return true;
	}

private:
	vec3 albedo;	
};
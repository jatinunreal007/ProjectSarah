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
	metal(const vec3 albedo, double fuzz)
		: albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1){}

	bool scatter(const Ray& rIncident, const HitRecord& rec, vec3& attenuation, Ray& scattered) const override
	{
		vec3 refl = vec3().Reflect(rIncident.GetDirection(), rec.normal);
		refl = vec3().Vec3Normalize(refl) +  RandomUnitVec3() * fuzz;
		scattered = Ray(rec.point, refl);
		attenuation = albedo;
		return vec3().Vec3Dot(scattered.GetDirection(), rec.normal);
	}

private:
	vec3 albedo;	
	double fuzz;
};

class Dielectric : public materials
{
public:
	Dielectric(double refractiveIndex)
		: refractiveIndex(refractiveIndex) {}

	bool scatter(const Ray& rIncident, const HitRecord& rec, vec3& attenuation, Ray& scattered) const override
	{
		attenuation = vec3(1.0, 1.0, 1.0);
		double RefractiveIndex = rec.frontFace ? 1 / refractiveIndex : refractiveIndex;
		vec3 UnitDir = vec3().Vec3Normalize(rIncident.GetDirection());
		double CosTheta = std::fmin(vec3().Vec3Dot(UnitDir * (-1), rec.normal), 1.0);
		double SinTheta = std::sqrt(1.0 - CosTheta * CosTheta);

		vec3 dir;
		if (RefractiveIndex * SinTheta > 1.0)
			dir = vec3().Reflect(UnitDir, rec.normal);

		else
			dir = vec3().Refract(UnitDir, rec.normal, RefractiveIndex);

		scattered = Ray(rec.point, dir);
		return true;
	}

private:
	double refractiveIndex;

};
#pragma once
#include "Vectors.h"
#include "Ray.h"
#include "Lightings.h"
#include "Utilities.h"
#include "Aabb.h"

class materials;

class HitRecord
{
public:
	vec3 point;
	vec3 normal;
	double t = 0.0;
	vec3 color;
	std::shared_ptr<materials> mat;
	bool frontFace = false;

	void SetFaceNormal(const Ray& r, const vec3 OutwardNormal)
	{
		frontFace = vec3::Vec3Dot(r.GetDirection(), OutwardNormal) < 0;
		normal = frontFace ? OutwardNormal : OutwardNormal*(-1);
	}
};



class Hittable
{
public:
	virtual ~Hittable() = default;
	virtual bool Hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const = 0;
	virtual AABB BoundingBox() const = 0;
};



class Sphere : public Hittable
{
public:
	Sphere(const vec3 c, double radius, std::shared_ptr<materials> mat)
		: StaticCentre(c), centre(c, vec3(0.0,0.0,0.0)) , radius(radius), mat(mat) 
	{
		auto rvec = vec3(radius, radius, radius);
		bbox = AABB(StaticCentre - rvec, StaticCentre + rvec);
	}

	Sphere(const vec3 centre1, const vec3 centre2, double radius, std::shared_ptr<materials> mat)
		: StaticCentre(centre1), centre(centre1, centre2 - centre1), radius(radius), mat(mat) 
	{
		auto rvec = vec3(radius, radius, radius);
		auto bbox1 = AABB(centre.at(0) - rvec, centre.at(0) + rvec);
		auto bbox2 = AABB(centre.at(1) - rvec, centre.at(1) + rvec);
		bbox = AABB(bbox1, bbox2);

	}

	bool Hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const override
	{
		vec3 CurrentCentre = centre.at(r.GetTime());
		vec3 OC = r.GetOrigin() - CurrentCentre;

		double a = vec3::Vec3Dot(r.GetDirection(), r.GetDirection());
		double b = 2.0 * vec3::Vec3Dot(OC, r.GetDirection());
		double c = vec3::Vec3Dot(OC, OC) - radius * radius;
		double discriminant = (b * b) - (4 * a * c);
		auto sqrtDiscriminant = std::sqrt(discriminant);

		if (discriminant < 0.0) 
		{
			return false;
		}
		auto root = (-b - sqrtDiscriminant) / (2.0 * a);
		if (root < tMin || root > tMax)
		{
			root = (-b + sqrtDiscriminant) / (2.0 * a);

			if (root < tMin || root > tMax)
				return false;
		}
		rec.t = root;
		rec.point = r.GetOrigin() + r.GetDirection() * rec.t;
		rec.normal = (rec.point - CurrentCentre) / radius;
		rec.color = (vec3(1.0, 0.3, 0.0));
		rec.SetFaceNormal(r, rec.normal);
		rec.mat = mat;
		return true;

	}

	AABB BoundingBox() const override
	{
		return bbox;
	}

	vec3 SphereGetCentre() const
	{
		return StaticCentre;
	}
	double SphereGetRadius() const
	{
		return radius;
	}

private:
	Ray centre;
	vec3 StaticCentre;
	double radius;
	AABB bbox;
	std::shared_ptr<materials> mat;
};

class Plane : public Hittable
{
public:
	Plane(vec3 PassingPoint, vec3 normal, std::shared_ptr<materials> mat)
		: PassingPoint(PassingPoint), normal(vec3::Vec3Normalize(normal)), mat(mat) 
	{
		bbox = AABB();
	}


	bool Hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const override
	{
		float A = vec3::Vec3Dot(normal, r.GetDirection());
		float B = vec3::Vec3Dot(normal, PassingPoint - r.GetOrigin());

		float t = B / A;
		if (A == 0)
			return false;

		if (t < tMin || t > tMax)
		{
			return false;
		}
		
		rec.t = t;
		rec.point = r.at(t);
		rec.normal = normal;
		rec.color = (vec3(0.5f, 0.5f, 0.5f));
		rec.mat = mat;
		return true;
	}

	AABB BoundingBox() const override
	{
		return bbox;
	}
	 
private:
	vec3 PassingPoint;
	vec3 normal;
	AABB bbox;
	std::shared_ptr<materials> mat;
};
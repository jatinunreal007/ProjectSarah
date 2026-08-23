#pragma once

#include "Hittables.h"
#include <vector>
#include <memory>

class HittablesList : public Hittable
{
public:
	HittablesList() {}
	std::vector<std::shared_ptr<Hittable>> objects;

	void Clear()
	{
		objects.clear();
	}

	void Add(std::shared_ptr<Hittable> object)
	{
		objects.emplace_back(object);
		bbox = AABB(bbox, object->BoundingBox());
	}

	bool Hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const override
	{
		HitRecord tempRec;
		bool hitAnything = false;
		auto closestSoFar = tMax;

		for (const auto& object : objects)
		{
			if (object->Hit(r, tMin, closestSoFar, tempRec))
			{
				hitAnything = true;
				closestSoFar = tempRec.t;
				rec = tempRec;
			}
		}

		return hitAnything;
	}

	AABB BoundingBox() const override
	{
		return bbox;
	}

private:
	AABB bbox;
};
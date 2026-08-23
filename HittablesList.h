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

	bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const override
	{
		HitRecord tempRec;
		bool hitAnything = false;
		auto closestSoFar = ray_t.GetMax();

		for (const auto& object : objects)
		{
			if (object->Hit(r, ray_t, tempRec))
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
#pragma once

#include "Aabb.h"
#include "Hittables.h"
#include "HittablesList.h"
#include <algorithm>

class BvhNode : public Hittable
{
public:
	BvhNode(HittablesList list)
		:BvhNode(list.objects, 0, list.objects.size())
	{
	}

	BvhNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end)
	{
		int axis = RandomInt(0, 2);
		auto  compare = (axis == 0) ? xBoxCompare : (axis == 1) ? yBoxCompare : zBoxCompare;

		size_t ObjectSpan = end - start;

		if (ObjectSpan == 1)
			left = right = objects[start];

		else if (ObjectSpan == 2)
		{
			left = objects[start];
			right = objects[start + 1];
		}
		else
		{
			std::sort(std::begin(objects) + start, std::begin(objects) + end, compare);
			auto mid = start + ObjectSpan / 2;
			left = std::make_shared<BvhNode>(objects, start, mid);
			right = std::make_shared<BvhNode>(objects, mid, end);
		}

		bbox = AABB(left->BoundingBox(), right->BoundingBox());
	}

	bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const override
	{
		if (!bbox.Hit(r, ray_t))
		{
			return false;
		}

		bool HitLeft = left->Hit(r, ray_t, rec);
		bool HitRight = right->Hit(r, Interval(ray_t.GetMin(), (HitLeft ? rec.t : ray_t.GetMax())), rec);

		return HitLeft || HitRight;
	}

	AABB BoundingBox() const override
	{
		return bbox;
	}

private:
	std::shared_ptr<Hittable> left;
	std::shared_ptr<Hittable> right;
	AABB bbox;

	static bool BoxCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b, int AxisIndex)
	{
		auto aAxisInterval = a->BoundingBox().AxisInterval(AxisIndex);
		auto bAxisInterval = b->BoundingBox().AxisInterval(AxisIndex);
		return aAxisInterval.GetMin() < bAxisInterval.GetMin();

	}

	static bool xBoxCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b)
	{
		return BoxCompare(a, b, 0);
	}
	static bool yBoxCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b)
	{
		return BoxCompare(a, b, 1);
	}
	static bool zBoxCompare(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b)
	{
		return BoxCompare(a, b, 2);
	}


};
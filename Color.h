#pragma once

#include<iostream>
#include "Vectors.h"
#include<string>
#include "Ray.h"
#include "Hittables.h"
#include<algorithm>
#include "utilities.h"
#include "HittablesList.h"

class Color:public vec3
{
public:
	Color()
		: vec3(0.0f, 0.0f, 0.0f) {
	}
	Color(float r, float g, float b)
		: vec3(r, g, b) {
	}
	std::string ColorOut(float r, float g, float b)
	{
		//std::cout << r << " " << g << " " << b <<"\n";
		std::string colorString = std::to_string(255.0 * r) + " " + std::to_string(255.0 * g) + " " + std::to_string(255.0 * b);
		return colorString;
	}
	std::string ColorOut(const vec3& color)
	{
		//std::cout << r << " " << g << " " << b <<"\n";
		std::string colorString = std::to_string(255.0 * color.x) + " " + std::to_string(255.0 * color.y) + " " + std::to_string(255.0 * color.z);
		return colorString;
	}

	void ColorOut(std::ostream& out, const vec3& PixelColor) {
		
		auto r = (PixelColor.x);
		auto g = (PixelColor.y);
		auto b = (PixelColor.z);

		//out << r << " " << g << " " << b << "\n";

		static const Interval ColorInterval(0.000, 0.999);

		int rbyte = int(256 * ColorInterval.clamp(r));
		int gbyte = int(256 * ColorInterval.clamp(g));
		int bbyte = int(256 * ColorInterval.clamp(b));

		out << rbyte << " " << gbyte << " " << bbyte << "\n";
	}
	

	vec3 vec3Color(const Color& color)
	{
		vec3 colour = vec3(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f);
		return colour;
	}
	//Finalising Color of the Objects--->
	vec3 RayColor(const Ray& ray, const Hittable& scene, const Light& pl1)
	{
		HitRecord rec;
		if (scene.Hit(ray,0.0f, 100000.0f, rec))
		{
			vec3 Normal = (rec.normal + vec3(1.0f, 1.0f, 1.0f)) * 0.5f; // from a range of -1 to 1 to a range of 0 to 1 to use it as a color. 
			float FinalColorFactor = vec3::Vec3Dot(Normal, pl1.PLightGetDirection());
			return vec3(1.0f, 0.65f, 0.0f) * FinalColorFactor * pl1.PLightGetIntensity();
		}

		vec3 UnitDirection = vec3().Vec3Normalize(ray.GetDirection());
		float lerp = 0.5f * (UnitDirection.y + 1.0f); // Lerp is a linear interpolation function that takes in a value between 0 and 1 and returns a value between two other values. In this case, we are using it to interpolate between white and blue based on the y component of the unit direction vector.
		// Here UnitDirection.y is from -1 to 1 , bcoz it a unit vector and we need to convert it to a value between 0 and 1 to use it in the lerp function. We do this by adding 1 to it and then dividing by 2.
		auto BlendColor = (vec3(1.0f, 1.0f, 1.0f) * (1.0f - lerp)) + (vec3(0.5f, 0.7f, 1.0f) * lerp);
		return BlendColor;
		//return vec3(0.0f, 0.0f, 0.0f);	
	}
private:
	float r,g,b;
};
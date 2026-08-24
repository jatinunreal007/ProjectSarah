#pragma once

#include<iostream>
#include "Vectors.h"
#include <string>
#include "Ray.h"
#include "Hittables.h"
#include <algorithm>

class Color:public vec4
{
public:
	std::string ColorOut(float r, float g, float b)
	{
		//std::cout << r << " " << g << " " << b <<"\n";
		std::string colorString = std::to_string(255.0 * r) + " " + std::to_string(255.0 * g) + " " + std::to_string(255.0 * b);
		return colorString;
	}
	void ColorOut(std::ostream& out, const vec3& PixelColor) {
		out << static_cast<int>(255.999 * PixelColor.x) << " "
			<< static_cast<int>(255.999 * PixelColor.y) << " "
			<< static_cast<int>(255.999 * PixelColor.z) << "\n";
	}

	vec4 Vec4Color(const Color& color)
	{
		vec4 colour = vec4(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f);
		return colour;
	}
	//Finalising Color of the Objects--->
	vec3 RayColor(const Ray& ray, Sphere& s1, PointLight& pl1)
	{
		
		if (s1.HitSphere(s1.SphereGetCentre(), s1.SphereGetRadius(), ray))
		{
			vec3 Normal = (s1.SphereGetNormal(ray) + vec3(1.0f, 1.0f, 1.0f)) * 0.5f; // from a range of -1 to 1 to a range of 0 to 1 to use it as a color. 
		    float FinalColorFactor = vec3::Vec3Dot(Normal, pl1.PLightGetDirection());
			return vec3(1.0f, 0.65f, 0.0f) * FinalColorFactor*pl1.PLightGetIntensity();
		}
		vec3 UnitDirection = vec3().Vec3Normalize(ray.GetDirection());
		float lerp = 0.5f * (UnitDirection.y + 1.0f); // Lerp is a linear interpolation function that takes in a value between 0 and 1 and returns a value between two other values. In this case, we are using it to interpolate between white and blue based on the y component of the unit direction vector.
		// Here UnitDirection.y is from -1 to 1 , bcoz it a unit vector and we need to convert it to a value between 0 and 1 to use it in the lerp function. We do this by adding 1 to it and then dividing by 2.
		auto BlendColor = (vec3(1.0f, 1.0f, 1.0f)* (1.0f - lerp)) + (vec3(0.5f, 0.7f, 1.0f) * lerp);
		return BlendColor;
		//return vec3(0.0f, 0.0f, 0.0f);	
	}
private:
	float r,g,b,a;
};
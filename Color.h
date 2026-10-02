#pragma once

#include <iostream>
#include "Vectors.h"
#include <string>
#include <algorithm>
#include "utilities.h"

class Color:public vec3
{
public:
	Color()
		:vec3(0.0f, 0.0f, 0.0f) {}

	Color(double r, double g, double b)
		: vec3(r,g,b) {}

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

	void ColorOut(std::ostream& out, const vec3& PixelColor) 
    {
		
		auto R = (PixelColor.x);
		auto G = (PixelColor.y);
		auto B = (PixelColor.z);

		//out << r << " " << g << " " << b << "\n";


		//gama corr
		auto r = std::sqrt(R);
		auto g = std::sqrt(G);
		auto b = std::sqrt(B);


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
	
private:
};

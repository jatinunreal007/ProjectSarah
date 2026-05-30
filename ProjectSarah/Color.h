#pragma once

#include<iostream>
#include "Vectors.h"
#include <string>

class Color:public vec4
{
public:
	std::string ColorOut(float r, float g, float b)
	{
		//std::cout << r << " " << g << " " << b <<"\n";
		std::string colorString = std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b);
		return colorString;
	}

	void Vec4Colour(const Color& color)
	{
		vec4 colour = vec4(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f);
	}

	
};
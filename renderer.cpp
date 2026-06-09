#include <iostream>
#include <fstream>
#include "Color.h"
#include "Camera.h"
#include "Ray.h"
#include "utilities.h"


int main()
{

	//Objects--->
	Sphere s1(0.0f, 0.0f, 2.0f, 0.5f);

	//Lightings--->
	PointLight pl1(vec3(0.5f, -0.5f, -0.02f), 2.0f);

	//Camera--->
	Camera c1;

	std::clog << "Starting Render...\n";
	c1.Render(s1, pl1);
	
	std::cin.get();
}

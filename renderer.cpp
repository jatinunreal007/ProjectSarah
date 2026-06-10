#include <iostream>
#include "Color.h"
#include <fstream>
#include "Camera.h"
#include "utilities.h"
#include "HittablesList.h"

int main()
{

	HittablesList scene;

	//Objects--->
	Sphere s1(vec3(0.0f, 0.0f, -2.0f), 0.5f);
	scene.Add(std::make_shared<Sphere>(s1));

	//Lightings--->
	Light pl1(vec3(-0.5f, 0.5f, 0.02f), 2.0f);

	//Camera--->
	Camera c1;

	std::clog << "Starting Render...\n";
	c1.Render(scene, pl1);
	
	std::cin.get();
}

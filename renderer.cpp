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
	Sphere s1(vec3(0.0f, 0.0f, 6.0f), 1.0f);
	scene.Add(std::make_shared<Sphere>(s1));

	
	Sphere s2(vec3(4.0f, 1.0f, 9.0f), 2.0f);
	scene.Add(std::make_shared<Sphere>(s2));

	Sphere s3(vec3(-1.25f, -0.6f, 5.0f), 0.4f);
	scene.Add(std::make_shared<Sphere>(s3));

	Plane p1(vec3(0.0f, -1.0f, -2.0f), vec3(0.0f, 1.0f, 0.0f));
	scene.Add(std::make_shared<Plane>(p1));


	//Lightings--->
	Light pl1(vec3(-0.5f, 0.5f, 0.02f), 2.0f); //directional light


	//Camera--->
	Camera c1;
	std::cout << "Starting Render....\n";
	c1.Render(scene, pl1);
	
	std::cin.get();
}
#include <iostream>
#include "Color.h"
#include <fstream>
#include "Camera.h"
#include "utilities.h"
#include "HittablesList.h"
#include "Materials.h"


int main()
{

	HittablesList scene;

	auto MaterialGround = std::make_shared<lambertian>(Color(0.76, 0.76, 0.76));
	auto MaterialS1 = std::make_shared<metal>(Color(0.95, 0.93, 0.88),0.1);
	auto MaterialS2 = std::make_shared<metal>(Color(0.83, 0.55, 0.37), 0.2);
	auto MaterialS3 = std::make_shared<metal>(Color(1.0, 0.78, 0.34), 0.05);


	//Objects--->
	Sphere s1(vec3(0.0f, 0.0f, 6.0f), 1.0f, MaterialS1);
	scene.Add(std::make_shared<Sphere>(s1));

	Sphere s2(vec3(4.0f, 1.0f, 9.0f), 2.0f, MaterialS2);
	scene.Add(std::make_shared<Sphere>(s2));

	Sphere s3(vec3(-1.25f, -0.6f, 5.0f), 0.4f, MaterialS3);
	scene.Add(std::make_shared<Sphere>(s3));

	Plane p1(vec3(0.0f, -1.0f, -2.0f), vec3(0.0f, 1.0f, 0.0f), MaterialGround);
	scene.Add(std::make_shared<Plane>(p1));


	//Lightings--->
	Light pl1(vec3(-0.5f, 0.5f, 0.02f), 2.0f); //directional light


	//Camera--->
	Camera c1;
	std::cout << "Starting Render....\n";
	c1.Render(scene, pl1);
	
	std::cin.get();
}
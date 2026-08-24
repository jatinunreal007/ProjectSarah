#include <iostream>
#include "Color.h"
#include <fstream>
#include "Camera.h"
#include "utilities.h"
#include "HittablesList.h"
#include "Materials.h"
#include "Bvh.h"


void ScatterRandomMaterial(HittablesList& scene)
{

	for (int i = 0; i < 500; i++)
	{
		auto MatChances = RandomDouble();
		std::shared_ptr<materials> mat;

		if (MatChances < 0.5)
		{
			auto albedo = RandomVec3() * RandomVec3();
			mat = std::make_shared<lambertian>(albedo);
			auto centre = vec3(RandomDouble(-1.0, 1.0) *30.0,-0.8, RandomDouble(-1.0, 1.0) *30.0);
			auto centre2 = centre + vec3(0, RandomDouble(0, 0.5), 0);
			scene.Add(std::make_shared<Sphere>(centre, centre2, 0.2, mat));
		}
		else if (MatChances >= 0.5 && MatChances < 0.85)
		{
			auto albedo = RandomVec3(0.5, 1);
			auto fuzz = RandomVec3(0, 0.5);
			mat = std::make_shared<metal>(albedo, 0.1);
			scene.Add(std::make_shared<Sphere>(vec3(RandomDouble(-1.0, 1.0)*30.0, -0.8, RandomDouble(-1.0, 1.0) *30.0), 0.2, mat));
		}
		else
		{
			mat = std::make_shared<Dielectric>(1.5);
			scene.Add(std::make_shared<Sphere>(vec3(RandomDouble(-1.0, 1.0) *30.0,-0.8, RandomDouble(-1.0, 1.0) *30.0), 0.2, mat));
		}
	}

}



int main()
{

	HittablesList scene;
	
	auto MaterialGround = std::make_shared<lambertian>(Color(0.76, 0.76, 0.76));
	auto MaterialS1 = std::make_shared<metal>(Color(0.95, 0.93, 0.88),0.05);
	auto MaterialS2 = std::make_shared<lambertian>(Color(0.88, 0.69, 0.72));
	auto MaterialS3 = std::make_shared<metal>(Color(1.0, 0.78, 0.34), 0.0);
	auto MaterialS4 = std::make_shared<Dielectric>(1.5f);

	//Objects--->
	Sphere s1(vec3(-1.5f, 0.0f, 2.0f), 1.0f,MaterialS1);
	scene.Add(std::make_shared<Sphere>(s1));

	Sphere s2(vec3(4.0f, 1.0f, -2.0f), 2.0f, MaterialS4);
	scene.Add(std::make_shared<Sphere>(s2));

	Sphere s3(vec3(-2.0f, -0.5f, 4.0f), 0.5f, MaterialS3);
	scene.Add(std::make_shared<Sphere>(s3));

	Sphere s4(vec3(0.5f, 0.5f, 0.0f), 1.5f, MaterialS2);
	scene.Add(std::make_shared<Sphere>(s4));

	Plane p1(vec3(0.0f, -1.0f, -2.0f), vec3(0.0f, 1.0f, 0.0f), MaterialGround);
	scene.Add(std::make_shared<Plane>(p1));

	ScatterRandomMaterial(scene);

	//Lightings--->
	Light pl1(vec3(-0.5f, 0.5f, 0.02f), 2.0f); //directional light

	BvhNode Bvh(scene);
	//Camera--->
	Camera c1;

	c1.CameraSetImageWidth(4096);
	c1.CameraSetSamples(512);
	c1.CameraSetFov(16);

	c1.CameraSetLookFrom(vec3(-2, 1, 20));
	c1.CameraSetLookAt(vec3(0.5,0.5,0.0));
	c1.CameraSetVup(vec3(0,1,0));

	c1.CameraSetDefocusAngle(1);
	c1.CameraSetFocusDistance(20.8);

	std::cout << "Starting Render....\n";


	c1.Render(Bvh, pl1);
	std::cin.get();
}
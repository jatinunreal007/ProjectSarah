#include <iostream>
#include <fstream>
#include "Color.h"
#include "Camera.h"
#include "Ray.h"
#include "Chrono"

//Hey Github

int main()
{
	//RayColor
	//Color().RayColor(Ray(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0)));

	//Objects--->
	Sphere s1(0.0f, 0.0f, 2.0f, 0.5f);

	//Lightings--->
	PointLight pl1(vec3(0.5f, -0.5f, -0.02f), 2.0f);

	//Aspect Ratio of Viewport
	int ImageWidth = 1600;
	int ImageHeight = (int)(ImageWidth / aspectRatio);
	ImageHeight = (ImageHeight < 1) ? 1 : ImageHeight;

	//Calculate the Horizontal and Vertical Vectors of the viewport
	vec3 ViewportHorizontal = vec3(Camera().CameraGetWidth(), 0.0, 0.0);
	vec3 ViewportVertical = vec3(0.0, Camera().CameraGetHeight(), 0.0);

	//Calculate the linear Deltas (i.e = small change or unit change) of the viewport
	vec3 HorizontalDelta = ViewportHorizontal / ImageWidth;
	vec3 VerticalDelta = (ViewportVertical / ImageHeight) * (-1); // Negative bcoz we need to move downwards in the viewport to fill the rows

	//Calculate the upper left corner of the viewport
	vec3 UpperLeftViewport = vec3(-Camera().CameraGetWidth() / 2 , Camera().CameraGetHeight() / 2, 1.0);
	vec3 UpperLeftPixel = (UpperLeftViewport) + (HorizontalDelta / 2) + (VerticalDelta / 2); // We need to move half a pixel right and half a pixel down to get the center of the upper left pixel


	std::ofstream render("render.ppm");

	render << "P3\n" << ImageWidth << " " << ImageHeight << "\n255\n";
	//Render function---->
	std::clog << "Starting Render...\n";

	auto start = std::chrono::high_resolution_clock::now();

	Camera c1;
	Color ColorUtil;

	for (int i = 0; i < ImageHeight ; i++)
	{
		std::clog << "Rendering row " << i << "\n";
		for (int j = 0; j < ImageWidth ; j++)
		{

		vec3 PixelCentre = UpperLeftPixel + (HorizontalDelta * j) + (VerticalDelta * i); // We need to move right by j pixels and down by i pixels to get the center of the pixel at (i,j)
		vec3 RayDirection = PixelCentre - c1.CameraGetOrigin(); 
		Ray r(c1.CameraGetOrigin(), RayDirection);
		vec3 PixelColor = ColorUtil.RayColor(r, s1, pl1);
		ColorUtil.ColorOut(render, PixelColor);          //----> by convention rows are filled first then columns
		// Color().ColorOut will either take floats from 0 to 1 representing rgb , or direct a vector
		}

	}

	//Image Info Output--->
	std::cout << "\nImage Width: " << ImageWidth << "\n";
	std::cout << "Image Height: " << ImageHeight << "\n";

	//Render Time Calculation--->
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsed = end - start;
	std::cout << "\nRender complete!\n";
	std::cout << "Time taken: " << elapsed.count() << " seconds.\n";
	

	render.close();

	std::cin.get();
}

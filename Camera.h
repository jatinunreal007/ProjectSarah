#pragma once

#include "Ray.h"
#include "Vectors.h"
#include "Chrono"

const double aspectRatio = 16.0 / 9.0;

class Camera
{
public:
	const double CameraGetHeight()
	{
		return viewport_height;
	}
	const double CameraGetWidth()
	{
		return viewport_width;
	}
	const vec3 CameraGetOrigin()
	{
		return vec3(0.0, 0.0, 0.0);
	}

	void InitializeViewport()
	{

		//Aspect Ratio of Viewport
		ImageWidth = 800;
		ImageHeight = (int)(ImageWidth / aspectRatio);
		ImageHeight = (ImageHeight < 1) ? 1 : ImageHeight;


		//Calculate the Horizontal and Vertical Vectors of the viewport
		 vec3 ViewportHorizontal = vec3(CameraGetWidth(), 0.0, 0.0);
		 vec3 ViewportVertical = vec3(0.0, CameraGetHeight(), 0.0);

		//Calculate the linear Deltas (i.e = small change or unit change) of the viewport
		 HorizontalDelta = ViewportHorizontal / ImageWidth;
		 VerticalDelta = (ViewportVertical / ImageHeight) * (-1); // Negative bcoz we need to move downwards in the viewport to fill the rows

		//Calculate the upper left corner of the viewport
		 vec3 UpperLeftViewport = vec3(-CameraGetWidth() / 2, CameraGetHeight() / 2, 1.0);
		 UpperLeftPixel = (UpperLeftViewport)+(HorizontalDelta / 2) + (VerticalDelta / 2); // We need to move half a pixel right and half a pixel down to get the center of the upper left pixel

	}
	//Render function---->

	void Render(const Sphere& s1, const PointLight& pl1)
	{

		InitializeViewport();
		std::ofstream render("render.ppm");


		//Image Info Output--->
		std::cout << "\nImage Width: " << ImageWidth << "\n";
		std::cout << "Image Height: " << ImageHeight << "\n";

		render << "P3\n" << ImageWidth << " " << ImageHeight << "\n255\n";

		Camera c1;
		Color ColorUtil;

		auto start = std::chrono::high_resolution_clock::now();

		for (int i = 0; i < ImageHeight; i++)
		{
			std::clog << "Rendering row " << i << "\n";
			for (int j = 0; j < ImageWidth; j++)
			{

				vec3 PixelCentre = UpperLeftPixel + (HorizontalDelta * j) + (VerticalDelta * i); // We need to move right by j pixels and down by i pixels to get the center of the pixel at (i,j)
				vec3 RayDirection = PixelCentre - CameraGetOrigin();
				Ray r(CameraGetOrigin(), RayDirection);
				vec3 PixelColor = ColorUtil.RayColor(r, s1, pl1);
				//for ()
				//{

				//}
				ColorUtil.ColorOut(render, PixelColor);          //----> by convention rows are filled first then columns
				// Color().ColorOut will either take floats from 0 to 1 representing rgb , or direct a vector
				//std::cout<<ColorUtil.ColorOut(PixelColor)<<std::endl;
			}

		}

		//Render Time Calculation--->
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed = end - start;
		std::cout << "\nRender complete!\n";
		std::cout << "Time taken: " << elapsed.count() << " seconds.\n";

		render.close();
	}

	//Ray GetRay()
	//{
	//	auto offset = SampleSquare();
	//	auto PixelSample = 
	//}
	vec3 SampleSquare()
	{
		return vec3(RandomDouble() - 0.5, RandomDouble() - 0.5, 0.0);
	}

private:
	double viewport_width = 2;
	double viewport_height = viewport_width / aspectRatio;
	const float FocalLength = 2.0f;
	int SamplePerpixel = 128;
	double PixelSampleScale = 1.0 / SamplePerpixel;

private:
	int ImageWidth;
	int ImageHeight;
	vec3 UpperLeftPixel;
	vec3 HorizontalDelta;
	vec3 VerticalDelta;
};
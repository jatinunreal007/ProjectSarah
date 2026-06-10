#pragma once

#include "Ray.h"
#include "Vectors.h"
#include "Chrono"

const double aspectRatio = 16.0 / 10.0;

class Camera
{
public:
	const double CameraGetHeight()
	{
		return ViewportHeight;
	}
	const double CameraGetWidth()
	{
		return ViewportWidth;
	}
	const vec3 CameraGetOrigin()
	{
		return vec3(0.0, 0.0, 0.0);
	}

	

	void InitializeViewport()
	{

		//Aspect Ratio of Viewport
		ImageWidth = 1600;
		ImageHeight = (int)(ImageWidth / aspectRatio);
		ImageHeight = (ImageHeight < 1) ? 1 : ImageHeight;

		PixelSampleScale = 1.0 / SamplePerpixel;

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

    Ray GetRay(int j, int i)
	{
		auto offset = SampleSquare();
		auto PixelSample = UpperLeftPixel
			+ (HorizontalDelta * (i + offset.x))
			+ (VerticalDelta * (j + offset.y));
		auto RayDirection = PixelSample - CameraGetOrigin();
		return Ray(CameraGetOrigin(), RayDirection);
	}

	vec3 SampleSquare()
	{
		return vec3(RandomDouble() - 0.5, RandomDouble() - 0.5, 0.0);
	}
	//Render function---->

	void Render(const Hittable& scene, const Light& pl1)
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
				Color PixelColor(0.0f, 0.0f, 0.0f);

				for (int k = 0; k < SamplePerpixel; k++)
				{
					Ray r = GetRay(i, j);
					PixelColor += ColorUtil.RayColor(r, scene, pl1);
					//std::cout<<ColorUtil.ColorOut(PixelColor)<<std::endl;
				}
				ColorUtil.ColorOut(render, PixelColor * PixelSampleScale);
			}

		}

		//Render Time Calculation--->
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed = end - start;
		std::cout << "\nRender complete!\n";
		std::cout << "Time taken: " << elapsed.count() << " seconds.\n";

		render.close();
	}

private:
	double ViewportWidth = 2;
	double ViewportHeight = ViewportWidth / aspectRatio;
	const float FocalLength = 2.0f;
	int SamplePerpixel = 256;
	double PixelSampleScale;

private:
	int ImageWidth;
	int ImageHeight;
	vec3 UpperLeftPixel;
	vec3 HorizontalDelta;
	vec3 VerticalDelta;
};
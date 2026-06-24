#pragma once

#include "Ray.h"
#include "Vectors.h"
#include <chrono>

const double aspectRatio = 1.9 / 1.0;

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
	const void CameraSetImageWidth(int w)
	{
		ImageWidth = w;
	}
	const void CameraSetSamples(int s)
	{
		SamplePerpixel = s;
	}

	const void CameraSetFov(int fov)
	{
		Vfov = fov;
	}

	void InitializeViewport()
	{

		//Aspect Ratio of Viewport
		//ImageWidth = 800;
		ImageHeight = (int)(ImageWidth / aspectRatio);
		ImageHeight = (ImageHeight < 1) ? 1 : ImageHeight;

		PixelSampleScale = 1.0 / SamplePerpixel;

		auto theta = DegreesToRadians(Vfov);
		auto h = std::tan(theta/2);
	    ViewportHeight = 2 * h * FocalLength;
		ViewportWidth = ViewportHeight * double((ImageWidth / ImageHeight));

		//Calculate the Horizontal and Vertical Vectors of the viewport
		 vec3 ViewportHorizontal = vec3(CameraGetWidth(), 0.0f, 0.0f);
		 vec3 ViewportVertical = vec3(0.0f,-CameraGetHeight(), 0.0f);

		//Calculate the linear Deltas (i.e = small change or unit change) of the viewport
		 HorizontalDelta = ViewportHorizontal / ImageWidth;
		 VerticalDelta = (ViewportVertical / ImageHeight); // Negative bcoz we need to move downwards in the viewport to fill the rows

		//Calculate the upper left corner of the viewport
		 vec3 UpperLeftViewport = vec3(-CameraGetWidth() / 2.0f, CameraGetHeight() / 2.0f, FocalLength);
		 UpperLeftPixel = (UpperLeftViewport)+(HorizontalDelta / 2.0f) + (VerticalDelta / 2.0f); // We need to move half a pixel right and half a pixel down to get the center of the upper left pixel

	}

    Ray GetRay(int row, int col)
	{
		auto offset = SampleSquare();
		auto PixelSample = UpperLeftPixel
			+ (HorizontalDelta * (col + offset.x))
			+ (VerticalDelta * (row + offset.y));
		auto RayDirection = PixelSample - CameraGetOrigin();
		return Ray(CameraGetOrigin(), RayDirection);
	}

	vec3 SampleSquare()
	{
		return vec3(RandomDouble() - 0.5f, RandomDouble() - 0.5f, 0.0f);
	}
	//Render function---->

	void Render(const Hittable& scene, const Light& pl1)
	{

		InitializeViewport();
		std::ofstream render("render.ppm");


		//Image Info Output--->
		std::cout << "\nImage Width: " << ImageWidth << std::endl;
		std::cout << "Image Height: " << ImageHeight << std::endl;

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
					PixelColor += ColorUtil.RayColor(r, scene, pl1, MaxDepth);
					//std::cout<<ColorUtil.ColorOut(PixelColor)<<std::endl;
				}
				ColorUtil.ColorOut(render, PixelColor * PixelSampleScale);
			}

		}

		//Render Time Calculation--->
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed = end - start;
		std::cout << "\nRender completed!\n";
		std::cout << "Time taken: " << elapsed.count() << " seconds.\n";

		render.close();
	}

private:
	double ViewportWidth;
	double ViewportHeight;
	const float FocalLength = 2.0f;
	int SamplePerpixel;
	double PixelSampleScale;
	int MaxDepth = 50;
	double Vfov;

private:
	int ImageWidth;
	int ImageHeight;
	vec3 UpperLeftPixel;
	vec3 HorizontalDelta;
	vec3 VerticalDelta;
};
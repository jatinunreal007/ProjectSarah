#pragma once

#include <execution>
#include <cmath>
#include "Ray.h"
#include "Vectors.h"
#include <chrono>
#include <numeric>
#include <iomanip>

double aspectRatio = 1.9 / 1.0;

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
		return Centre;
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
	const void CameraSetLookFrom(const vec3& point)
	{
		LookFrom = point;
	}

	const void CameraSetLookAt(const vec3& dir)
	{
		LookAt = dir;
	}
	const void CameraSetVup(const vec3& vup)
	{
		Vup = vup;
	}
	const void CameraSetDefocusAngle(double angle)
	{
		DefocusAngle = angle;
	}
	const void CameraSetFocusDistance(double dist)
	{
		FocusDistance = dist;
	}

	void InitializeViewport()
	{

		//Aspect Ratio of Viewport
		//ImageWidth = 800;
		ImageHeight = (int)(ImageWidth / aspectRatio);
		ImageHeight = (ImageHeight < 1) ? 1 : ImageHeight;

		PixelSampleScale = 1.0 / SamplePerpixel;

		Centre = LookFrom;

		//setting up Viewport Dimensions
		auto theta = DegreesToRadians(Vfov);
		auto h = std::tan(theta/2);
	    ViewportHeight = 2 * h * FocusDistance;
		ViewportWidth = ViewportHeight * ((double(ImageWidth) / double(ImageHeight)));

		//Cam Orientation
		w = vec3::Vec3Normalize(LookFrom - LookAt);
		u = vec3::Vec3Normalize(vec3::Vec3Cross(Vup, w));
		v = vec3::Vec3Cross(w, u);


		//Calculate the Horizontal and Vertical Vectors of the viewport
		vec3 ViewportHorizontal =  u * ViewportWidth;
		vec3 ViewportVertical = v * ViewportHeight * (-1);

		//Calculate the linear Deltas (i.e = small change or unit change) of the viewport
		 HorizontalDelta = ViewportHorizontal / ImageWidth;
		 VerticalDelta = (ViewportVertical / ImageHeight); // Negative bcoz we need to move downwards in the viewport to fill the rows

		//Calculate the upper left corner of the viewport
		 vec3 UpperLeftViewport = Centre - (w * FocusDistance) - ViewportHorizontal/2 - ViewportVertical/2 ;
		 UpperLeftPixel = (UpperLeftViewport)+(HorizontalDelta / 2.0f) + (VerticalDelta / 2.0f); // We need to move half a pixel right and half a pixel down to get the center of the upper left pixel

		 auto DefocusRadius = FocusDistance * std::tan(DegreesToRadians(DefocusAngle) / 2);
		 DefocusDiskX = u * DefocusRadius;
		 DefocusDiskY = v * DefocusRadius;
	}

    Ray GetRay(int row, int col)
	{
		auto offset = SampleSquare();
		auto PixelSample = UpperLeftPixel
			+ (HorizontalDelta * (col + offset.x))
			+ (VerticalDelta * (row + offset.y));
		auto RayOrigin = (DefocusAngle <= 0) ? Centre : DefocusDiskSample();
		auto RayDirection = PixelSample - RayOrigin;
		return Ray(RayOrigin, RayDirection);
	}

	vec3 SampleSquare()
	{
		return vec3(RandomDouble() - 0.5f, RandomDouble() - 0.5f, 0.0f);
	}

	vec3 DefocusDiskSample() const
	{
		auto v = RandomUnitVec3OnDisk();
		return Centre + (DefocusDiskX * v.x) + (DefocusDiskY * v.y);
	}
	//Render function---->

	//void Render(const Hittable& scene, const Light& pl1)
	//{

	//	InitializeViewport();
	//	std::ofstream render("render.ppm");


	//	//Image Info Output--->
	//	std::cout << "\nImage Width: " << ImageWidth << std::endl;
	//	std::cout << "Image Height: " << ImageHeight << std::endl;

	//	render << "P3\n" << ImageWidth << " " << ImageHeight << "\n255\n";

	//	Camera c1;
	//	Color ColorUtil;

	//	auto start = std::chrono::high_resolution_clock::now();

	//	for (int i = 0; i < ImageHeight; i++)
	//	{
	//		std::clog << "Rendering row " << i << "\n";
	//		for (int j = 0; j < ImageWidth; j++)
	//		{
	//			Color PixelColor(0.0f, 0.0f, 0.0f);

	//			for (int k = 0; k < SamplePerpixel; k++)
	//			{
	//				Ray r = GetRay(i, j);
	//				PixelColor += ColorUtil.RayColor(r, scene, pl1, MaxDepth);
	//				//std::cout<<ColorUtil.ColorOut(PixelColor)<<std::endl;
	//			}
	//			ColorUtil.ColorOut(render, PixelColor * PixelSampleScale);
	//		}

	//	}



	//	//Render Time Calculation--->
	//	auto end = std::chrono::high_resolution_clock::now();
	//	std::chrono::duration<float> elapsed = end - start;
	//	std::cout << "\nRender completed!\n";
	//	std::cout << "Time taken: " << elapsed.count() << " seconds.\n";

	//	render.close();
	//}

	void Render(const Hittable& scene, const Light& pl1)
	{
		InitializeViewport();

		std::cout << "\nImage Width: " << ImageWidth << std::endl;
		std::cout << "Image Height: " << ImageHeight << std::endl;

		std::vector<int> pixelBuffer(ImageWidth * ImageHeight * 3);
		Color ColorUtil;
		std::vector<int> rows(ImageHeight);
		std::iota(rows.begin(), rows.end(), 0);

		const Interval ci(0.0, 0.999);

		std::atomic<int> rowsDone(0);  // thread-safe counter
		auto start = std::chrono::high_resolution_clock::now();

		std::for_each(std::execution::par, rows.begin(), rows.end(), [&](int i)
			{
				for (int j = 0; j < ImageWidth; j++)
				{
					Color PixelColor(0.0f, 0.0f, 0.0f);
					for (int k = 0; k < SamplePerpixel; k++)
					{
						Ray r = GetRay(i, j);
						PixelColor += ColorUtil.RayColor(r, scene, pl1, MaxDepth);
					}

					auto scaled = PixelColor * PixelSampleScale;
					int idx = (i * ImageWidth + j) * 3;
					pixelBuffer[idx] = int(256 * ci.clamp(std::sqrt(scaled.x)));
					pixelBuffer[idx + 1] = int(256 * ci.clamp(std::sqrt(scaled.y)));
					pixelBuffer[idx + 2] = int(256 * ci.clamp(std::sqrt(scaled.z)));
				}

				// Each thread increments when its row finishes
				int done = ++rowsDone;
				if (done % 10 == 0 || done == ImageHeight)  // print every 10 rows to avoid spam
				{
					float percent = (float)done / ImageHeight * 100.0f;
					auto now = std::chrono::high_resolution_clock::now();
					float elapsed = std::chrono::duration<float>(now - start).count();
					float eta = (elapsed / done) * (ImageHeight - done);

					std::clog << std::fixed << std::setprecision(1)
						<< percent << "% | Row " << done << "/" << ImageHeight
						<< " | Elapsed: " << int(elapsed) << "s"
						<< " | ETA: " << int(eta) << "s\n";
				}
			});

		std::clog << "\n";

		auto end = std::chrono::high_resolution_clock::now();
		float totalTime = std::chrono::duration<float>(end - start).count();
		std::cout << "Render completed in " << totalTime << "s\n";

		std::ofstream render("render.ppm");
		render << "P3\n" << ImageWidth << " " << ImageHeight << "\n255\n";
		for (int i = 0; i < ImageWidth * ImageHeight; i++)
			render << pixelBuffer[i * 3] << " " << pixelBuffer[i * 3 + 1] << " " << pixelBuffer[i * 3 + 2] << "\n";
	}


private:
	double ViewportWidth;
	double ViewportHeight;
	vec3 Centre;
	float FocalLength;
	int SamplePerpixel;
	double PixelSampleScale;
	int MaxDepth = 50;

	double Vfov;
	vec3 LookFrom = vec3(0, 0, 0);
	vec3 LookAt = vec3(0,0,-1);
	vec3 Vup = vec3(0, 1, 0);

	vec3 u, v, w;

public:
	double DefocusAngle;
	double FocusDistance;

private:
	int ImageWidth;
	int ImageHeight;

	vec3 UpperLeftPixel;

	vec3 HorizontalDelta;
	vec3 VerticalDelta;

	vec3 DefocusDiskX;
	vec3 DefocusDiskY;
};
#pragma once

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
private:
	double viewport_width = 2;
	double viewport_height = viewport_width / aspectRatio;
	const float FocalLength = 2.0f;
};
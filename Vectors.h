#pragma once
#include <iostream>
#include "Algebra.h"

class vec4
{
public:
	vec4() 
		: x(0), y(0), z(0), w(0) {}

	vec4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w) {}

	void VecOut()
	{
		std::cout << x << " " << y << " " << z << " " << w << "\n";
	}

protected:
	double x, y, z, w;
};

class vec3
{
public:
	vec3() 
		: x(0), y(0), z(0) {}

	vec3(double x, double y, double z)
		: x(x), y(y), z(z) {}

	vec3 operator-(const vec3& other) const
	{
		return vec3(x - other.x, y - other.y, z - other.z);
	}
	vec3 operator+(const vec3& other) const
	{
		return vec3(x + other.x, y + other.y, z + other.z);
	}

	vec3 operator+=(const vec3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	vec3 operator*(double scalar) const
	{
		return vec3(x * scalar, y * scalar, z * scalar);
	}
	vec3 operator/(double scalar) const
	{
		return vec3(x / scalar, y / scalar, z / scalar);
	}
	vec3 operator*(const vec3& other) const
	{
		return vec3(x * other.x, y * other.y, z * other.z);
	}

	double operator[](int i) const 
	{
		if (i == 0) return x;
		else if (i == 1) return y;
		else if (i == 2) return z;
	}       
	double& operator[](int i) 
	{
		if (i == 0) return x;
		else if (i == 1) return y;
		else if (i == 2) return z;
	}

	static double Vec3Dot(const vec3& v1, const vec3& v2)
	{
		double result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		return result;
	}
	static double Vec3Magnitude(const vec3& v)
	{
		double magnitude = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		return magnitude;
	}
	static vec3 Vec3Cross(const vec3& v1, const vec3& v2)
	{
		return vec3(v1.y * v2.z - v1.z * v2.y,
					v1.z * v2.x - v1.x * v2.z,
					v1.x * v2.y - v1.y * v2.x);
	}

	static vec3 Vec3Normalize(const vec3& v)
	{
		float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		if (length == 0) {
			return vec3(0, 0, 0); 
		}
		return vec3(v.x / length, v.y / length, v.z / length);
	}
	bool Vec3NearZero() const
	{
		const float s = 1e-8;
		return (std::fabs(x) < s) && (std::fabs(y) < s) && (std::fabs(z) < s);
	}

	inline vec3 Reflect(const vec3& incident, const vec3& normal)
	{
		return incident - normal * 2 * Vec3Dot(incident, normal);
	}
	inline vec3 Refract(const vec3& UnitvecI, const vec3& normal, double InvRelRefIndex)
	{
		auto CosTheta = std::fmin(Vec3Dot(UnitvecI * -1, normal), 1.0);
		vec3 Rperp = (UnitvecI + normal * CosTheta) * InvRelRefIndex;
		vec3 Rpll = normal * -std::sqrt(std::fabs(1.0 - Vec3Dot(Rperp, Rperp)));
		return Rperp + Rpll;
	}

public:
	double x, y, z;
};
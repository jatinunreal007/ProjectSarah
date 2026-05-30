#pragma once
#include<iostream>
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
	float x, y, z, w;
};

class vec3
{
public:
	vec3() 
		: x(0), y(0), z(0) {}

	vec3(float x, float y, float z)
		: x(x), y(y), z(z) {}

	vec3 operator-(const vec3& other) const
	{
		return vec3(x - other.x, y - other.y, z - other.z);
	}
	vec3 operator*(float scalar) const
	{
		return vec3(x * scalar, y * scalar, z * scalar);
	}
	vec3 operator/(float scalar) const
	{
		return vec3(x / scalar, y / scalar, z / scalar);
	}
	float Vec3Dot(const vec3& v1, const vec3& v2)
	{
		float result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		return result;
	}
	vec3 Vec3Cross(const vec3& v1, const vec3& v2)
	{
		matrice4 m1(v1.y, v1.z, v2.y, v2.z);
		matrice4 m2(v1.x, v1.z, v2.x, v2.z);
		matrice4 m3(v1.x, v1.y, v2.x, v2.y);

		vec3 result = vec3(m1.m4det(), -m2.m4det(), m3.m4det());
		return result;
	}

private:
	float x, y, z;
};
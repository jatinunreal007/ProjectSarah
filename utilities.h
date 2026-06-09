#pragma once

#include <iostream>
#include <limits>
#include <memory>
#include <numbers>
#include <random>

inline double DegreesToRadians(double degrees) {
	return degrees * std::numbers::pi / 180.0;
}

inline double RandomDouble() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> DoubleDist(0.0, 1.0);
	return DoubleDist(gen);
}

inline double RandomDouble(double min, double max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> DoubleDist(min, max);
	return DoubleDist(gen);
}

class Interval
{
public:
	Interval(double min, double max)
		: min(min), max(max) 
	{
	}

	bool surrounds(double x) const {
		return min < x && x < max;
	}

	double clamp(double val) const
	{ 
		return val < min ? min : val > max ? max : val;
	}

private:
	double min, max;
};

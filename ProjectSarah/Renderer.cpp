#include <iostream>
#include <fstream>
#include "Color.h"
int main()
{
	int width = 256;
	int height = 256;

	std::ofstream render("render.ppm");

	render << "P3\n" << width << " " << height << "\n255\n";

	for (int i = 0; i < height ; i++)
	{
		std::clog << "Rendering row " << i +1 << "\n";

		for (int j = 0; j < height; j++)
		{
			render << Color().ColorOut(j, 255-i, 0.0f) << "\n";; // first 256 will be in 1st row , by convention rows are filled first then columns
		}
	}

	render.close();
	std::cin.get();
}
#pragma once

class matrice2
{
public:
	matrice2(float a11, float a12, float a21, float a22)
		: a11(a11), a12(a12), a21(a21), a22(a22)
	{	

	};

	float m4det()
	{
		float determinant = (a11 * a22) - (a12 * a21);
		return determinant;
	}
private:
	float a11, a12, a21, a22;
};
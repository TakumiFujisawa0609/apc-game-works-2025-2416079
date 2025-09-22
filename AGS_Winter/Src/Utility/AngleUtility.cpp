#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include "Utility.h"
#include "AngleUtility.h"


double AngleUtility::Rad2DegD(double rad)
{
	return rad * (180.0 / DX_PI);
}

float AngleUtility::Rad2DegF(float rad)
{
	return rad * (180.0f / DX_PI_F);
}

int AngleUtility::Rad2DegI(int rad)
{
	return rad * Utility::Round(180.0f / DX_PI_F);
}

double AngleUtility::Deg2RadD(double deg)
{
	return deg * (DX_PI / 180.0);
}

float AngleUtility::Deg2RadF(float deg)
{
	return deg * (DX_PI_F / 180.0f);
}

int AngleUtility::Deg2RadI(int deg)
{
	return deg * Utility::Round(DX_PI_F / 180.0f);
}

MATRIX AngleUtility::GetMatrixRotateXYZ(const VECTOR& euler)
{
	MATRIX ret = MGetIdent();
	ret = MMult(ret, MGetRotX(euler.x));
	ret = MMult(ret, MGetRotY(euler.y));
	ret = MMult(ret, MGetRotZ(euler.z));
	return ret;
}

MATRIX AngleUtility::Multiplication(const VECTOR& child, const VECTOR& parent)
{
	return MMult(GetMatrixRotateXYZ(child), GetMatrixRotateXYZ(parent));
}

MATRIX AngleUtility::Multiplication(const MATRIX& child, const MATRIX& parent)
{
	return MMult(child, parent);
}

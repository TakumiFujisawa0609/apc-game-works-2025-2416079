#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include "Utility.h"
#include "VectorUtility.h"


double VectorUtility::Magnitude(const VECTOR& v)
{
    return sqrt(SqrMagnitude(v));
}

float VectorUtility::MagnitudeF(const VECTOR& v)
{
    return sqrtf(SqrMagnitudeF(v));
}

float VectorUtility::SqrMagnitudeF(const VECTOR& v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

double VectorUtility::SqrMagnitude(const VECTOR& v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

double VectorUtility::Distance(const VECTOR& v1, const VECTOR& v2)
{
    return sqrt(SqrDistance(v1, v2));
}

double VectorUtility::SqrDistance(const VECTOR& v1, const VECTOR& v2)
{
    return pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2);
}

bool VectorUtility::Equals(const VECTOR& v1, const VECTOR& v2)
{
    if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
    {
        return true;
    }
    return false;
}

bool VectorUtility::EqualsVZero(const VECTOR& v1)
{
    const VECTOR& v2 = Utility::VECTOR_ZERO;
    if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)
    {
        return true;
    }
    return false;
}

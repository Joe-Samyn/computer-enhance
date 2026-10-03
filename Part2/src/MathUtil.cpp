#include "MathUtil.h"

#include <math.h>

double DegreesToRadians(double degrees)
{
    return degrees * (M_PI/ 180);
}

double SinSquared(double degree) 
{
    return sin(degree) * sin(degree);
}

double Min(double a, double b)
{
    return a > b ? b : a;
}
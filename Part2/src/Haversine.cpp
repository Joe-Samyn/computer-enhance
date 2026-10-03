
#include "Haversine.h"
#include "MathUtil.h"

#include <math.h>


double HaversineDistance(Entry entry)
{
    double x0Radians = DegreesToRadians(entry.x0);
    double y0Radians = DegreesToRadians(entry.y0);
    double x1Radians = DegreesToRadians(entry.x1);
    double y1Radians = DegreesToRadians(entry.y1);

    double deltaLat = x1Radians - x0Radians;
    double deltaLon = y1Radians - y0Radians;

    double latitudeHaversine = SinSquared(deltaLat / 2);
    double longitudeHaversine = SinSquared(deltaLon / 2);
    
    double a = latitudeHaversine + (cos(x0Radians) * cos(x1Radians) * longitudeHaversine);
    double c = 2 * asin(Min(1, sqrt(a)));
    double distance = c * EARTH_RADIUS_KILOMETERS;

    return distance;

}
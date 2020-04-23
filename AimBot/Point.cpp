#include <math.h>
#include "Point.h"

float Point::distance(const Point& p)const
{
    return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y) + (z - p.z) * (z - p.z));
}


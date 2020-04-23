#pragma once

class Point
{
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    float distance(const Point& p) const;
};

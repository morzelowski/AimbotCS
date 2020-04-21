#pragma once
#include "Point.h"

struct Player
{
public:
    int hp;
    int team;
    bool isSpotted;
    Point pos;
    Point ang;
    bool strzelono = false;
};

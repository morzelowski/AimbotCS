#pragma once
#include "Point.h"

struct Player
{
public:
    int hp = 0;
    int team = 0;
    bool isSpotted = false;
    Point pos;
    Point ang;
    bool shooted = false;
};

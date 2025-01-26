#pragma once

#include <SFML/Graphics.hpp>

#include "CellType.h"

struct Wall
{
    CellType type;
    sf::RectangleShape shape;
};
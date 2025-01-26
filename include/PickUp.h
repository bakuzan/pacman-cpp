#pragma once

#include <SFML/Graphics.hpp>

#include "CellType.h"

struct PickUp
{
    CellType type;
    sf::CircleShape shape;
};
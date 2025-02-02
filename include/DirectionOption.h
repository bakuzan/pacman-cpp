#pragma once

#include <SFML/Graphics.hpp>

#include "Direction.h"

struct DirectionOption
{
    Direction direction;
    sf::Vector2f offset;
};
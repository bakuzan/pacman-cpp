#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <unordered_map>

#include "Direction.h"

namespace Constants
{
    extern const float WINDOW_SIZE;
    extern const float GRID_OFFSET_Y;
    extern const float GRID_WIDTH;
    extern const float GRID_HEIGHT;
    extern const float SPRITE_SIZE;

    extern const float HOUSE_CELL_LOWEST_Y;
    extern const float HOUSE_CELL_HIGHEST_Y;
    extern const sf::Vector2f LEFT_MOST_HOUSE_CELL;
    extern const sf::Vector2f RIGHT_MOST_HOUSE_CELL;
    extern const sf::Vector2f FIRST_OUTSIDE_CELL_LHS;

    extern const std::unordered_map<Direction, Direction> REVERSE_DIRECTION_MAP;

    extern const float COLLISION_TOLERANCE;

    extern const sf::Color WALL_COLOUR;
}

#endif // CONSTANTS_H

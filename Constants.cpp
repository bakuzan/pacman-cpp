#include "pch.h"

#include <unordered_map>

#include "include/Constants.h"
#include "include/Direction.h"

namespace Constants
{
    const float WINDOW_SIZE = 512.0f;
    const float GRID_OFFSET_Y = 3.0f;
    const float GRID_WIDTH = 29.0f;
    const float GRID_HEIGHT = 31.0f + GRID_OFFSET_Y + 1.0f; // Grid + Offset + Space Beneath
    const float SPRITE_SIZE = 1.0f;

    const float HOUSE_CELL_LOWEST_Y = GRID_OFFSET_Y + 13.0f;
    const float HOUSE_CELL_HIGHEST_Y = GRID_OFFSET_Y + 15.0f;
    const sf::Vector2f LEFT_MOST_HOUSE_CELL = sf::Vector2f(12, GRID_OFFSET_Y + 14);
    const sf::Vector2f RIGHT_MOST_HOUSE_CELL = sf::Vector2f(17, GRID_OFFSET_Y + 14);
    const sf::Vector2f FIRST_OUTSIDE_CELL_LHS = sf::Vector2f(13, GRID_OFFSET_Y + 11);

    const std::unordered_map<Direction, Direction> REVERSE_DIRECTION_MAP = {
        {UP, DOWN},
        {DOWN, UP},
        {LEFT, RIGHT},
        {RIGHT, LEFT}};

    const std::vector<sf::Vector2f> NO_UP_INTERSECTIONS = {
        sf::Vector2f(12, GRID_OFFSET_Y + 11),
        sf::Vector2f(15, GRID_OFFSET_Y + 11),
        sf::Vector2f(12, GRID_OFFSET_Y + 23),
        sf::Vector2f(15, GRID_OFFSET_Y + 23),
    };

    const float COLLISION_TOLERANCE = 0.09f; // 0.05f;

    const sf::Color WALL_COLOUR = sf::Color(33, 33, 222);
}

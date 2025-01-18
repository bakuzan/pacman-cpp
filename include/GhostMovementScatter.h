#include <SFML/Graphics.hpp>

#include "GhostPersonality.h"

#ifndef GHOSTMOVEMENTSCATTER_H
#define GHOSTMOVEMENTSCATTER_H

class GhostMovementScatter
{
public:
    static sf::Vector2f GetTargetTileForPersonality(GhostPersonality personality, const std::vector<sf::RectangleShape> &walls)
    {
        switch (personality)
        {
        case BLINKY:
        case PINKY:
        case INKY:
        case CLYDE:
        default:
            return sf::Vector2f(0, 0);
        }
    }
};

#endif // GHOSTMOVEMENTSCATTER_H

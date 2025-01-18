#include <SFML/Graphics.hpp>

#include "GhostPersonality.h"

#ifndef GHOSTMOVEMENTCHASE_H
#define GHOSTMOVEMENTCHASE_H

class GhostMovementChase
{
public:
    static sf::Vector2f GetTargetTileForPersonality(GhostPersonality personality, sf::Vector2f pacmanPosition)
    {
        switch (personality)
        {
        case BLINKY:
        case PINKY:
        case INKY:
        case CLYDE:
        default:
            return pacmanPosition;
        }
    }

private:
};

#endif // GHOSTMOVEMENTCHASE_H

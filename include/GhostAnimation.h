#ifndef GHOSTANIMATION_H
#define GHOSTANIMATION_H

#include <SFML/Graphics.hpp>

#include "Direction.h"
#include "GhostMode.h"

class GhostAnimation
{
public:
    GhostAnimation(sf::Texture *texture, float switchTime, float columnIndex);
    ~GhostAnimation();

    void Update(GhostMode mode, Direction direction, float frightTimeElapsed);

public:
    sf::IntRect textureRect;
    sf::IntRect defaultRect;

private:
    int columnIndex;
    sf::Vector2u currentImage;

    float totalTime;
    float switchTime;
};

#endif // GHOSTANIMATION_H

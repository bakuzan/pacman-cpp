#include <SFML/Graphics.hpp>

#include "Direction.h"

#ifndef GHOSTANIMATION_H
#define GHOSTANIMATION_H

class GhostAnimation
{
public:
    GhostAnimation(sf::Texture *texture, float switchTime, float columnIndex);
    ~GhostAnimation();

    void Update(Direction direction, float deltaTime);

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

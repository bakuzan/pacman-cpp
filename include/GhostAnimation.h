#ifndef GHOSTANIMATION_H
#define GHOSTANIMATION_H

#include <SFML/Graphics.hpp>

#include "Direction.h"
#include "GhostMode.h"

class GhostAnimation
{
public:
    GhostAnimation(sf::Texture *texture, float blinkInterval, float columnIndex);
    ~GhostAnimation();

    void Update(GhostMode mode, Direction direction, float deltaTime, float frightTimeElapsed);
    void Reset();

public:
    sf::IntRect textureRect;
    sf::IntRect defaultRect;

private:
    int columnIndex;
    sf::Vector2u currentImage;

    float blinkTimeElapsed;
    float blinkInterval;

private:
    void Init();
    void UpdateFrightenedSprite(float deltaTime, float frightTimeElapsed);
};

#endif // GHOSTANIMATION_H

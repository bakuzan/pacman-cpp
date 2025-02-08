#ifndef PLAYERANIMATION_H
#define PLAYERANIMATION_H

#include <SFML/Graphics.hpp>

#include "Direction.h"

class PlayerAnimation
{
public:
    PlayerAnimation(sf::Texture *texture, float switchTime);
    ~PlayerAnimation();

    void Update(Direction direction, float deltaTime);

public:
    sf::IntRect textureRect;
    sf::IntRect defaultRect;

private:
    sf::Vector2u currentImage;

    float totalTime;
    float switchTime;
    bool returningToDefault;
};

#endif // PLAYERANIMATION_H
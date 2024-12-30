#include <SFML/Graphics.hpp>

#include "Direction.h"

#ifndef ANIMATION_H
#define ANIMATION_H

class Animation
{
public:
    Animation(sf::Texture *texture, float switchTime);
    ~Animation();

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

#endif // ANIMATION_H
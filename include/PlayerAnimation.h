#ifndef PLAYERANIMATION_H
#define PLAYERANIMATION_H

#include <SFML/Graphics.hpp>

#include "Direction.h"

class PlayerAnimation
{
public:
    PlayerAnimation(sf::Texture *texture, float switchTime);
    ~PlayerAnimation();

    bool NextDeathFrame();
    void Update(Direction direction, float deltaTime);
    void Reset();

public:
    sf::IntRect textureRect;
    sf::IntRect defaultRect;

private:
    sf::Vector2u currentImage;
    int deathFrame;

    float totalTime;
    float switchTime;
    bool returningToDefault;

private:
    void Init();
};

#endif // PLAYERANIMATION_H
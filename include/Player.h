#include <SFML/Graphics.hpp>

#include "Direction.h"

#ifndef PLAYER_H
#define PLAYER_H

class Player
{
public:
    Player(sf::Image &spritesheet, float spriteSize);
    ~Player();

    void Draw(sf::RenderWindow &window);
    void SetPosition(float x, float y);
    void Update(float deltaTime);

private:
    sf::Sprite sprite;
    sf::Vector2f velocity;
    Direction direction;
};

#endif // PLAYER_H

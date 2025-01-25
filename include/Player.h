#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

#include "PlayerAnimation.h"
#include "Direction.h"

class Player
{
public:
    Player(sf::Texture &sharedTexture, float spriteSize);
    ~Player();

    sf::Vector2f GetPosition() const;
    Direction GetDirection() const;
    float GetSpeed() const;

    void Draw(sf::RenderWindow &window);
    void SetDirection(Direction newDirection);
    void SetPosition(float x, float y);
    void Update(Direction newDirection, float deltaTime, const std::vector<sf::RectangleShape> &walls, float minX, float maxX);

private:
    sf::Texture &texture;
    sf::Sprite sprite;
    float speed;
    sf::Vector2f velocity;
    Direction direction;
    PlayerAnimation animation;

private:
    void UpdateVelocity(Direction newDirection, float deltaTime);
};

#endif // PLAYER_H

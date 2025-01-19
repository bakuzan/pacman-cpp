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

    sf::Vector2f GetPosition() const { return sprite.getPosition(); };
    Direction GetDirection() const { return this->direction; };
    float GetSpeed() const { return this->speed; };

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
    void UpdateVelocity(Direction newDirection, float deltaTime)
    {
        velocity.x = 0.0f;
        velocity.y = 0.0f;

        switch (newDirection)
        {
        case Direction::LEFT:
            velocity.x -= speed * deltaTime;
            break;
        case Direction::UP:
            velocity.y -= speed * deltaTime;
            break;
        case Direction::RIGHT:
            velocity.x += speed * deltaTime;
            break;
        case Direction::DOWN:
            velocity.y += speed * deltaTime;
            break;
        default:
            // No movement, do nothing
            break;
        }
    }
};

#endif // PLAYER_H

#include <SFML/Graphics.hpp>
#include <cmath>

#include "Animation.h"
#include "Direction.h"
#include "Wall.h"

#ifndef PLAYER_H
#define PLAYER_H

class Player
{
public:
    Player(sf::Image &spritesheet, float spriteSize);
    ~Player();

    void Draw(sf::RenderWindow &window);
    void SetDirection(Direction newDirection);
    void SetPosition(float x, float y);
    void Update(float deltaTime, const std::vector<sf::RectangleShape> &walls);

private:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f velocity;
    Direction direction;
    Animation animation;

private:
    bool CheckTileCollision(sf::Sprite player, const std::vector<sf::RectangleShape> &walls)
    {
        for (const auto &wall : walls)
        {
            if (player.getGlobalBounds().intersects(wall.getGlobalBounds()))
            {
                return true;
            }
        }

        return false;
    }
    void UpdateVelocity(Direction newDirection)
    {
        float speed = 5.0f;
        velocity.x = 0.0f;
        velocity.y = 0.0f;

        switch (newDirection)
        {
        case Direction::LEFT:
            velocity.x -= speed;
            break;
        case Direction::UP:
            velocity.y -= speed;
            break;
        case Direction::RIGHT:
            velocity.x += speed;
            break;
        case Direction::DOWN:
            velocity.y += speed;
            break;
        default:
            // No movement, do nothing
            break;
        }
    }
    bool CanMove(Direction newDirection, float deltaTime, const std::vector<sf::RectangleShape> &walls)
    {
        sf::Vector2f position = sprite.getPosition();
        UpdateVelocity(newDirection);
        sf::Vector2f newPosition = position + (velocity * deltaTime);

        sf::Sprite tempSprite = sprite;
        tempSprite.setPosition(newPosition);
        return !CheckTileCollision(tempSprite, walls);
    }
};

#endif // PLAYER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

#include "Animation.h"
#include "Direction.h"

#ifndef PLAYER_H
#define PLAYER_H

class Player
{
public:
    Player(sf::Image &spritesheet, float spriteSize);
    ~Player();

    sf::Vector2f GetPosition() { return sprite.getPosition(); };

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
        sf::Vector2f playerPosition = player.getPosition();
        sf::Vector2f playerScale = player.getScale();
        sf::FloatRect playerSize = player.getLocalBounds();
        sf::Vector2f playerHalfSize = sf::Vector2f(0.5f * playerSize.width * playerScale.x, 0.5f * playerSize.height * playerScale.y);

        // No need to round the player position as we're using exact floats
        float playerMinX = playerPosition.x - playerHalfSize.x;
        float playerMaxX = playerPosition.x + playerHalfSize.x;
        float playerMinY = playerPosition.y - playerHalfSize.y;
        float playerMaxY = playerPosition.y + playerHalfSize.y;

        for (const auto &other : walls)
        {
            sf::Vector2f otherPosition = other.getPosition();
            sf::Vector2f otherHalfSize = sf::Vector2f(0.5f, 0.5f);

            float otherMinX = otherPosition.x - otherHalfSize.x;
            float otherMaxX = otherPosition.x + otherHalfSize.x;
            float otherMinY = otherPosition.y - otherHalfSize.y;
            float otherMaxY = otherPosition.y + otherHalfSize.y;

            bool collisionX = playerMaxX > otherMinX && playerMinX < otherMaxX;
            bool collisionY = playerMaxY > otherMinY && playerMinY < otherMaxY;

            if (collisionX && collisionY)
            {
                std::cout << "Player ("
                          << playerPosition.x << ", "
                          << playerPosition.y << ")\n";
                std::cout << "Checking collision with wall at position ("
                          << otherPosition.x << ", "
                          << otherPosition.y << ")\n";
                std::cout << "Player bounds X: [" << playerMinX << ", " << playerMaxX << "]\n";
                std::cout << "Player bounds Y: [" << playerMinY << ", " << playerMaxY << "]\n";
                std::cout << "Wall bounds X: [" << otherMinX << ", " << otherMaxX << "]\n";
                std::cout << "Wall bounds Y: [" << otherMinY << ", " << otherMaxY << "]\n";
                std::cout << "Collision detected with wall at position ("
                          << otherPosition.x << ", "
                          << otherPosition.y << ")\n";
                std::cout << std::endl;
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

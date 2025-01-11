#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

#include "PlayerAnimation.h"
#include "Direction.h"

#ifndef PLAYER_H
#define PLAYER_H

class Player
{
public:
    Player(sf::Texture &sharedTexture, float spriteSize);
    ~Player();

    sf::Vector2f GetPosition() { return sprite.getPosition(); };

    void Draw(sf::RenderWindow &window);
    void SetDirection(Direction newDirection);
    void SetPosition(float x, float y);
    void Update(Direction newDirection, float deltaTime, const std::vector<sf::RectangleShape> &walls, float minX, float maxX);

private:
    sf::Texture &texture;
    sf::Sprite sprite;
    sf::Vector2f velocity;
    Direction direction;
    PlayerAnimation animation;

private:
    bool CheckTileCollision(sf::Sprite player, const std::vector<sf::RectangleShape> &walls, sf::Vector2f &collisionOffset)
    {
        float tolerance = 0.05f;
        sf::FloatRect playerBounds = player.getGlobalBounds();
        sf::FloatRect intersection;

        collisionOffset = sf::Vector2f(0.0f, 0.0f);

        for (const auto &wall : walls)
        {
            sf::FloatRect wallBounds = wall.getGlobalBounds();

            // Check if the player bounds intersect with any wall
            if (playerBounds.intersects(wallBounds, intersection))
            {
                if (intersection.width < tolerance && intersection.height < tolerance)
                {
                    continue;
                }

                // Use the intersection to determine which side is blocking
                if (intersection.width < intersection.height)
                {
                    // Horizontal collision
                    if (playerBounds.left < wallBounds.left)
                    {
                        collisionOffset.x = intersection.width; // Collision on the right
                        // std::cout << "Horizontal collision on the right\n";
                    }
                    else
                    {
                        collisionOffset.x = -intersection.width; // Collision on the left
                        // std::cout << "Horizontal collision on the left\n";
                    }
                }
                else
                {
                    // Vertical collision
                    if (playerBounds.top < wallBounds.top)
                    {
                        collisionOffset.y = intersection.height; // Collision below
                        // std::cout << "Vertical collision below\n";
                    }
                    else
                    {
                        collisionOffset.y = -intersection.height; // Collision above
                        // std::cout << "Vertical collision above\n";
                    }
                }
                return true;
            }
        }

        return false;
    }
    void UpdateVelocity(Direction newDirection, float deltaTime)
    {
        float speed = 5.0f;
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

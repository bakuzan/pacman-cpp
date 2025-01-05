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
        float tolerance = 0.05f;
        sf::FloatRect playerBounds = player.getGlobalBounds();
        sf::FloatRect intersection;

        for (const auto &wall : walls)
        {
            sf::FloatRect wallBounds = wall.getGlobalBounds();

            // Check if the player bounds intersect with any wall
            if (playerBounds.intersects(wallBounds, intersection))
            {
                if (intersection.width < tolerance &&
                    intersection.height < tolerance)
                {
                    continue;
                }

                // Use the intersection to determine which side is blocking
                if (intersection.width < intersection.height)
                {
                    std::cout << "Intersection wxh: "
                              << intersection.width
                              << "x"
                              << intersection.height
                              << "\n";
                    // Horizontal collision
                    if (velocity.x > 0)
                    {
                        std::cout << "Horizontal collision on the right\n";
                        return true;
                    }
                    else if (velocity.x < 0)
                    {
                        std::cout << "Horizontal collision on the left\n";
                        return true;
                    }
                }
                else
                {
                    // Vertical collision
                    if (velocity.y > 0)
                    {
                        std::cout << "Vertical collision below\n";
                        return true;
                    }
                    else if (velocity.y < 0)
                    {
                        std::cout << "Vertical collision above\n";
                        return true;
                    }
                }
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

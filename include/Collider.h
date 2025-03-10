#ifndef COLLIDER_H
#define COLLIDER_H

#include <SFML/Graphics.hpp>

#include "Constants.h"
#include "Wall.h"

class Collider
{
public:
    static bool CheckTileCollision(sf::Sprite spriteEntity, const std::vector<Wall> &walls, sf::Vector2f &collisionOffset)
    {
        sf::FloatRect spriteBounds = spriteEntity.getGlobalBounds();
        sf::FloatRect intersection;

        collisionOffset = sf::Vector2f(0.0f, 0.0f);

        for (const auto &wall : walls)
        {
            sf::FloatRect wallBounds = wall.shape.getGlobalBounds();

            // Check if the player bounds intersect with any wall
            if (spriteBounds.intersects(wallBounds, intersection))
            {
                if (intersection.width < Constants::COLLISION_TOLERANCE &&
                    intersection.height < Constants::COLLISION_TOLERANCE)
                {
                    continue;
                }

                // Use the intersection to determine which side is blocking
                if (intersection.width < intersection.height)
                {
                    // Horizontal collision
                    if (spriteBounds.left < wallBounds.left)
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
                    if (spriteBounds.top < wallBounds.top)
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
};

#endif // COLLIDER_H

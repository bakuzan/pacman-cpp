#include <SFML/Graphics.hpp>
#include <cmath>
#include <unordered_map>
#include <vector>

#include "Collider.h"
#include "GhostAnimation.h"
#include "GhostMode.h"
#include "GhostPersonality.h"
#include "GhostModeController.h"
#include "GhostMovementChase.h"
#include "GhostMovementScatter.h"
#include "Player.h"

#ifndef GHOST_H
#define GHOST_H

class Ghost
{
public:
    Ghost(sf::Texture &sharedTexture, float spriteSize, int spriteSheetColumnIndex);
    ~Ghost();

    sf::Vector2f GetPosition() { return sprite.getPosition(); };

    void Draw(sf::RenderWindow &window);
    void SetPosition(float x, float y);
    void Update(float deltaTime, const std::vector<sf::RectangleShape> &walls, const Player &player, float minX, float maxX);

private:
    sf::Texture texture;
    sf::Sprite sprite;
    GhostAnimation animation;

    GhostPersonality personality;
    float speed;

    GhostModeController *mode;

    Direction currentDirection;

private:
    sf::Vector2f GetTargetTile(GhostMode currentMode, const std::vector<sf::RectangleShape> &walls, const Player &player)
    {
        switch (currentMode)
        {
        case GhostMode::SCATTER:
            return GhostMovementScatter::GetTargetTileForPersonality(personality, walls);
        case GhostMode::CHASE:
            return GhostMovementChase::GetTargetTileForPersonality(personality, player.GetPosition());
        default:
            return sf::Vector2f();
        }
    }
    float calculateDistance(sf::Vector2f a, sf::Vector2f b)
    {
        return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
    }
    Direction DetermineDirection(float deltaTime, const std::vector<sf::RectangleShape> &walls, Direction lastMovedDirection, sf::Sprite ghost, sf::Vector2f targetPosition, sf::Vector2f &collisionOffset)
    {
        // Possible directions
        std::vector<Direction> directions = {UP, LEFT, DOWN, RIGHT};

        // Map to store reverse directions
        std::unordered_map<Direction, Direction> reverseDirectionMap = {
            {UP, DOWN},
            {DOWN, UP},
            {LEFT, RIGHT},
            {RIGHT, LEFT}};

        // Exclude reverse direction
        Direction reverseDirection = reverseDirectionMap[lastMovedDirection];
        directions.erase(std::remove(directions.begin(),
                                     directions.end(),
                                     reverseDirection),
                         directions.end());

        float epsilon = 1e-5;
        float minDistance = std::numeric_limits<float>::max();
        Direction selectedDirection = lastMovedDirection;
        std::unordered_map<Direction, int> priorityMap = {{UP, 1}, {LEFT, 2}, {DOWN, 3}, {RIGHT, 4}};
        sf::Vector2f currentPosition = ghost.getPosition();
        sf::Vector2f localCollisionOffset;

        for (Direction direction : directions)
        {
            sf::Vector2f velocity = GetDirectionVector(deltaTime, direction);
            sf::Vector2f nextPosition = currentPosition + velocity;
            ghost.setPosition(nextPosition);

            bool willCollide = Collider::CheckTileCollision(ghost, walls, localCollisionOffset);
            if (willCollide && direction != lastMovedDirection)
            {
                continue;
            }
            else if (willCollide && direction == lastMovedDirection)
            {
                sf::Vector2f diff = velocity - localCollisionOffset;
                if (std::abs(diff.x) <= epsilon && std::abs(diff.y) <= epsilon)
                {
                    continue;
                }
            }

            float distance = calculateDistance(nextPosition, targetPosition);

            if (distance < minDistance)
            {
                minDistance = distance;
                selectedDirection = direction;
                collisionOffset = localCollisionOffset;
            }
            else if (distance == minDistance)
            {
                if (priorityMap[direction] < priorityMap[selectedDirection])
                {
                    selectedDirection = direction;
                    collisionOffset = localCollisionOffset;
                }
            }
        }

        return selectedDirection;
    }
    sf::Vector2f GetDirectionVector(float deltaTime, Direction direction)
    {
        switch (direction)
        {
        case UP:
            return sf::Vector2f(0, -1) * speed * deltaTime;
        case DOWN:
            return sf::Vector2f(0, 1) * speed * deltaTime;
        case LEFT:
            return sf::Vector2f(-1, 0) * speed * deltaTime;
        case RIGHT:
            return sf::Vector2f(1, 0) * speed * deltaTime;
        default:
            return sf::Vector2f(0, 0) * speed * deltaTime;
        }
    }
};

#endif // GHOST_H

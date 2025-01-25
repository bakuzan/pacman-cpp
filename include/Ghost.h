#ifndef GHOST_H
#define GHOST_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <unordered_map>
#include <vector>

#include "Collider.h"
#include "GhostAnimation.h"
#include "GhostMode.h"
#include "GhostPersonality.h"
#include "GhostModeController.h"
#include "GhostMovement.h"
#include "Player.h"

class Ghost
{
public:
    Ghost(sf::Texture &sharedTexture, float spriteSize, int spriteSheetColumnIndex);
    ~Ghost();

    sf::Vector2f GetPosition() const;
    GhostPersonality GetPersonality() const;
    Direction GetDirection() const;

    void Draw(sf::RenderWindow &window);
    void SetPosition(float x, float y);
    void Update(float deltaTime, const std::vector<sf::RectangleShape> &walls, const std::vector<Ghost> &ghosts, const Player &player, float minX, float maxX);

private:
    sf::Texture texture;
    sf::Sprite sprite;
    GhostAnimation animation;

    GhostPersonality personality;
    float speed;

    GhostModeController *mode;

    Direction currentDirection;

private:
    float CalculateDistance(sf::Vector2f a, sf::Vector2f b);
    Direction DetermineDirection(float deltaTime, const std::vector<sf::RectangleShape> &walls, GhostMode mode, Direction lastMovedDirection, sf::Sprite ghost, sf::Vector2f targetPosition, sf::Vector2f &collisionOffset);
    void ExcludeDirections(const GhostMode &mode, const Direction &lastMovedDirection, std::vector<Direction> &directions);
    bool Ghost::IsGhostDoor(const sf::RectangleShape &wall);
};

#endif // GHOST_H

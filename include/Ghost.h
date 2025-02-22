#ifndef GHOST_H
#define GHOST_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <unordered_map>
#include <vector>

#include "Collider.h"
#include "DirectionOption.h"
#include "GhostAnimation.h"
#include "GhostMode.h"
#include "GhostPersonality.h"
#include "GhostModeController.h"
#include "GhostMovement.h"
#include "Player.h"
#include "Wall.h"

class Ghost
{
public:
    Ghost(sf::Texture &sharedTexture, float spriteSize, int spriteSheetColumnIndex);
    ~Ghost();

    sf::Vector2f GetSpawnPosition() const;
    sf::Vector2f GetPosition() const;
    GhostPersonality GetPersonality() const;
    Direction GetDirection() const;

    void Draw(sf::RenderWindow &window);
    void SetSpawnPosition(float x, float y);
    void Update(float deltaTime, const std::vector<Wall> &walls, const std::vector<Ghost> &ghosts, const Player &player, float minX, float maxX);
    void Reset();

private:
    sf::Texture &texture;
    sf::Sprite sprite;
    GhostAnimation animation;

    GhostPersonality personality;
    sf::Vector2f spawnPosition;
    float speed;

    GhostModeController *mode;

    Direction lastKnownDirection;
    GhostMode lastKnownMode;

private:
    float CalculateDistance(sf::Vector2f a, sf::Vector2f b);
    Direction DetermineDirection(GhostMode mode, sf::Sprite ghost, sf::Vector2f targetPosition, const std::vector<DirectionOption> &validDirections, bool forceReverseDirection, sf::Vector2f &collisionOffset);
    std::vector<DirectionOption> GetMoveableDirections(float deltaTime, const std::vector<Wall> &walls, GhostMode mode, float movementSpeed, Direction lastMovedDirection, sf::Sprite ghost, bool forceReverseDirection);

    float GetMovementSpeed(GhostMode mode, const sf::Vector2f &position, float maxX);
    std::vector<Wall> FilteredWalls(GhostMode mode, const std::vector<Wall> &walls);
    DirectionOption PickDirection(const std::vector<DirectionOption> &directions, Direction currentDirection);
};

#endif // GHOST_H

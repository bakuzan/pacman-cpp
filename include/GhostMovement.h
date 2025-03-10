#ifndef GHOSTMOVEMENT_H
#define GHOSTMOVEMENT_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>

#include "Direction.h"
#include "GhostMode.h"
#include "GhostPersonality.h"
#include "Player.h"
#include "Wall.h"

class Ghost;

class GhostMovement
{
public:
    GhostMovement();
    ~GhostMovement();

    static sf::Vector2f GetTargetTile(GhostPersonality personality, GhostMode currentMode, const std::vector<Wall> &walls, const std::vector<Ghost> &ghosts, const Player &player, float deltaTime);

    static sf::Vector2f GetDirectionVector(Direction direction, float speed, float deltaTime);

    static std::vector<Direction> GetDirections(GhostMode mode, Direction lastMovedDirection, const sf::Vector2f &position, bool forceReverseDirection);

    static bool IsAtIntersection(const sf::Vector2f &position, float margin = 0.1f);

private:
    static std::pair<float, float> GetMaxXY(const std::vector<Wall> &walls);

    static sf::Vector2f GetHousedTargetTile(GhostPersonality personality, const std::vector<Ghost> &ghosts);

    static sf::Vector2f GetScatterTargetTile(GhostPersonality personality, const std::vector<Wall> &walls);

    static sf::Vector2f GetChaseTargetTile(GhostPersonality personality, const std::vector<Wall> &walls, const std::vector<Ghost> &ghosts, const Player &player, float deltaTime);

    static sf::Vector2f GetSpawnTargetTile(GhostPersonality personality, const std::vector<Ghost> &ghosts);
};

#endif // GHOSTMOVEMENT_H

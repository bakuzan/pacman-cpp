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

private:
    static std::pair<float, float> GetMaxXY(const std::vector<Wall> &walls);

    static sf::Vector2f GetHousedTargetTile(GhostPersonality personality, const std::vector<Ghost> &ghosts);

    static sf::Vector2f GetScatterTargetTile(GhostPersonality personality, const std::vector<Wall> &walls);

    static sf::Vector2f GetChaseTargetTile(GhostPersonality personality, const std::vector<Wall> &walls, const std::vector<Ghost> &ghosts, const Player &player, float deltaTime);
};

#endif // GHOSTMOVEMENT_H

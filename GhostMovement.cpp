#include "include/Ghost.h"
#include "include/GhostMovement.h"

GhostMovement::GhostMovement()
{
    // Constructor
}

GhostMovement::~GhostMovement()
{
    // Destructor
}

sf::Vector2f GhostMovement::GetTargetTile(GhostPersonality personality, GhostMode currentMode, const std::vector<sf::RectangleShape> &walls, const std::vector<Ghost> &ghosts, const Player &player, float deltaTime)
{
    switch (currentMode)
    {
    case GhostMode::SCATTER:
        return GetScatterTargetTile(personality, walls);
    case GhostMode::CHASE:
        return GetChaseTargetTile(personality, walls, ghosts, player, deltaTime);
    default:
        return sf::Vector2f();
    }
}

sf::Vector2f GhostMovement::GetDirectionVector(Direction direction, float speed, float deltaTime)
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

std::pair<float, float> GhostMovement::GetMaxXY(const std::vector<sf::RectangleShape> &walls)
{
    float maxX = std::numeric_limits<float>::min();
    float maxY = std::numeric_limits<float>::min();

    for (const auto &wall : walls)
    {
        sf::Vector2f position = wall.getPosition();
        maxX = std::max({maxX, position.x});
        maxY = std::max({maxY, position.y});
    }

    return {maxX, maxY};
}

sf::Vector2f GhostMovement::GetScatterTargetTile(GhostPersonality personality, const std::vector<sf::RectangleShape> &walls)
{
    auto [maxX, maxY] = GetMaxXY(walls);
    switch (personality)
    {
    case BLINKY:
        return sf::Vector2f(maxX - 2.0f, 0);
    case PINKY:
        return sf::Vector2f(2.0f, 0);
    case INKY:
        return sf::Vector2f(maxX, maxY + 2.0f);
    case CLYDE:
        return sf::Vector2f(0, maxY + 2.0f);
    default:
        return sf::Vector2f(0, 0);
    }
}

sf::Vector2f GhostMovement::GetChaseTargetTile(GhostPersonality personality, const std::vector<sf::RectangleShape> &walls, const std::vector<Ghost> &ghosts, const Player &player, float deltaTime)
{
    sf::Vector2f pacmanPosition = player.GetPosition();
    Direction pacmanDirection = player.GetDirection();
    float pacmanSpeed = player.GetSpeed();

    switch (personality)
    {
    case PINKY:
        return pacmanPosition + (GhostMovement::GetDirectionVector(pacmanDirection, pacmanSpeed, deltaTime) * 4.0f);
    case INKY:
    {
        auto blinky = std::find_if(ghosts.cbegin(), ghosts.cend(), [](const Ghost &ghost)
                                   { return ghost.GetPersonality() == BLINKY; });
        sf::Vector2f blinkyPosition = blinky->GetPosition();
        sf::Vector2f vec1 = pacmanPosition + (GhostMovement::GetDirectionVector(pacmanDirection, pacmanSpeed, deltaTime) * 2.0f);
        sf::Vector2f vec2 = (vec1 - blinkyPosition) * 2.0f;
        return blinkyPosition + vec2;
    }
    case CLYDE:
    {
        auto clyde = std::find_if(ghosts.cbegin(), ghosts.cend(), [](const Ghost &ghost)
                                  { return ghost.GetPersonality() == CLYDE; });
        sf::Vector2f d = pacmanPosition - clyde->GetPosition();
        float ds = d.x * d.x + d.y * d.y;
        if (ds <= 64.0f) // 64 is (TILEWIDTH * 8) ^ 2
        {
            return GetScatterTargetTile(personality, walls);
        }
        else
        {
            return pacmanPosition + (GhostMovement::GetDirectionVector(pacmanDirection, pacmanSpeed, deltaTime) * 4.0f);
        }
    }
    case BLINKY:
    default:
        return pacmanPosition;
    }
}

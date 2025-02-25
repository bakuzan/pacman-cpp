#include "pch.h"

#include <cstdlib>
#include <stdexcept>

#include "include/CellType.h"
#include "include/Constants.h"
#include "include/DirectionOption.h"
#include "include/Ghost.h"
#include "include/GhostModeController.h"
#include "include/GhostMovement.h"
#include "include/Player.h"
#include "include/SFMLUtils.h"
#include "include/Wall.h"

#include "include/EnumUtils.h"

Ghost::Ghost(sf::Texture &sharedTexture, float spriteSize, int spriteSheetColumnIndex)
    : texture(sharedTexture), animation(&sharedTexture, 0.2f, spriteSheetColumnIndex)
{
    sprite.setTexture(sharedTexture);
    sprite.setTextureRect(animation.defaultRect);
    SFMLUtils::CenterOriginAndScale(sprite, spriteSize);

    personality = static_cast<GhostPersonality>(spriteSheetColumnIndex);
    speed = personality == GhostPersonality::BLINKY ||
                    personality == GhostPersonality::CLYDE
                ? 5.0
                : 4.8;

    lastKnownDirection = Direction::NONE;
    mode = GhostModeController::GetInstance();

    // std::cout << "SpriteSheetColumnIndex: " << spriteSheetColumnIndex << "\n";
    // std::cout << "Texture Rect: " << sprite.getTextureRect().left << ", "
    //           << sprite.getTextureRect().top << ", "
    //           << sprite.getTextureRect().width << ", "
    //           << sprite.getTextureRect().height << "\n";
}

Ghost::~Ghost()
{
    // Destructor
}

sf::Vector2f Ghost::GetSpawnPosition() const
{
    return spawnPosition;
}

sf::Vector2f Ghost::GetPosition() const
{
    return sprite.getPosition();
}

GhostPersonality Ghost::GetPersonality() const
{
    return personality;
}

Direction Ghost::GetDirection() const
{
    return lastKnownDirection;
}

void Ghost::Update(float deltaTime, const std::vector<Wall> &walls, const std::vector<Ghost> &ghosts, const Player &player, float minX, float maxX)
{
    GhostMode currentMode = mode->GetMode(personality);
    GhostMode currentModeIgnoringFright = mode->GetModeIgnoringFrightened(personality);
    bool forceReverseDirection = mode->CheckForcedReverseQueue(personality);
    bool justExitedTheHouse = lastKnownMode == GhostMode::LEAVING &&
                              currentModeIgnoringFright != GhostMode::LEAVING;
    bool aboutToEnterTheHouse = lastKnownMode != GhostMode::ENTERING &&
                                currentModeIgnoringFright == GhostMode::ENTERING;

    sf::Vector2f currentPosition = sprite.getPosition();
    sf::Vector2f collisionOffset;

    GhostMode calcMode = currentModeIgnoringFright == GhostMode::HOUSED ||
                                 currentModeIgnoringFright == GhostMode::LEAVING ||
                                 currentModeIgnoringFright == GhostMode::SPAWN ||
                                 currentModeIgnoringFright == GhostMode::ENTERING
                             ? currentModeIgnoringFright
                             : currentMode;

    float movementSpeed = GetMovementSpeed(currentMode, currentPosition, maxX);
    sf::Vector2f targetPosition = GhostMovement::GetTargetTile(personality, calcMode, walls, ghosts, player, deltaTime);

    Direction newDirection;
    if (justExitedTheHouse)
    {
        newDirection = Direction::LEFT; // After leaving the house, ghosts always go left first
    }
    else if (aboutToEnterTheHouse)
    {
        newDirection = Direction::DOWN; // Ghost needs to go into the house, so force down
    }
    else
    {
        std::vector<DirectionOption> directions = GetMoveableDirections(
            deltaTime,
            walls,
            calcMode,
            movementSpeed,
            lastKnownDirection,
            sprite,
            forceReverseDirection);

        if ((calcMode != GhostMode::SCATTER &&
             calcMode != GhostMode::CHASE) ||
            GhostMovement::IsAtIntersection(currentPosition))
        {
            newDirection = DetermineDirection(
                calcMode,
                sprite,
                targetPosition,
                directions,
                forceReverseDirection,
                collisionOffset);
        }
        else
        {
            auto moveableDirection = PickDirection(directions, lastKnownDirection);
            newDirection = moveableDirection.direction;
            collisionOffset = moveableDirection.offset;
        }
    }

    // Set position and offset for determined direction
    sf::Vector2f newPosition = currentPosition + GhostMovement::GetDirectionVector(newDirection, movementSpeed, deltaTime);
    sprite.setPosition(newPosition);
    sprite.move(-collisionOffset);

    // Do we need to "teleport"?
    if (newPosition.x < minX ||
        newPosition.x > maxX)
    {
        newPosition.x = newPosition.x < minX
                            ? maxX
                            : minX;
        sprite.setPosition(newPosition);
    }

    // Animate
    GhostMode visualMode = currentModeIgnoringFright == GhostMode::SPAWN ||
                                   currentModeIgnoringFright == GhostMode::ENTERING
                               ? currentModeIgnoringFright
                               : currentMode;
    animation.Update(visualMode, newDirection, deltaTime, mode->GetFrightenedTimer());
    sprite.setTextureRect(animation.textureRect);

    // Update to new current values
    lastKnownDirection = newDirection;
    lastKnownMode = calcMode;
}

void Ghost::SetSpawnPosition(float x, float y)
{
    sprite.setPosition(x, y);
    spawnPosition = sf::Vector2f(x, y);
}

void Ghost::Draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}

void Ghost::Reset()
{
    lastKnownDirection = Direction::NONE;
    mode->ResetToHouse(personality);
    animation.Reset();
    sprite.setTextureRect(animation.defaultRect);
    sprite.setPosition(spawnPosition);
}

// Private

float Ghost::CalculateDistance(sf::Vector2f a, sf::Vector2f b)
{
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

Direction Ghost::DetermineDirection(GhostMode mode, sf::Sprite ghost, sf::Vector2f targetPosition, const std::vector<DirectionOption> &validDirections, bool forceReverseDirection, sf::Vector2f &collisionOffset)
{
    float minDistance = std::numeric_limits<float>::max();
    std::unordered_map<Direction, int> priorityMap = {{UP, 1}, {LEFT, 2}, {DOWN, 3}, {RIGHT, 4}};

    Direction selectedDirection = Direction::NONE;
    sf::Vector2f currentPosition = ghost.getPosition();

    // Frightened ghosts: Select random direction from the valid options
    if (mode == GhostMode::FRIGHTENED)
    {
        int randomIndex = std::rand() % validDirections.size();
        DirectionOption option = validDirections[randomIndex];
        selectedDirection = option.direction;
        collisionOffset = option.offset;
    }
    else
    {
        for (auto &vd : validDirections)
        {
            float distance = CalculateDistance(vd.position, targetPosition);

            if (distance < minDistance)
            {
                minDistance = distance;
                selectedDirection = vd.direction;
                collisionOffset = vd.offset;
            }
            else if (distance == minDistance)
            {
                if (priorityMap[vd.direction] < priorityMap[selectedDirection])
                {
                    selectedDirection = vd.direction;
                    collisionOffset = vd.offset;
                }
            }
        }
    }

    if (selectedDirection == Direction::NONE)
    {
        throw std::runtime_error("selectedDirection cannot be NONE");
    }

    return selectedDirection;
}

std::vector<DirectionOption> Ghost::GetMoveableDirections(float deltaTime, const std::vector<Wall> &walls, GhostMode mode, float movementSpeed, Direction lastMovedDirection, sf::Sprite ghost, bool forceReverseDirection)
{
    float epsilon = 1e-5;
    std::vector<Wall> filteredWalls = FilteredWalls(mode, walls);
    Direction selectedDirection = lastMovedDirection;
    sf::Vector2f currentPosition = ghost.getPosition();
    sf::Vector2f localCollisionOffset;

    std::vector<DirectionOption> validDirections;
    std::vector<Direction> directions = GhostMovement::GetDirections(
        mode,
        lastKnownDirection,
        currentPosition,
        forceReverseDirection);

    for (Direction direction : directions)
    {
        sf::Vector2f velocity = GhostMovement::GetDirectionVector(direction, movementSpeed, deltaTime);
        sf::Vector2f nextPosition = currentPosition + velocity;
        ghost.setPosition(nextPosition);

        bool willCollide = Collider::CheckTileCollision(ghost, filteredWalls, localCollisionOffset);
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

        validDirections.push_back({direction, nextPosition, localCollisionOffset});
    }

    return validDirections;
}

std::vector<Wall> Ghost::FilteredWalls(GhostMode mode, const std::vector<Wall> &walls)
{
    std::vector<Wall> filteredWalls(walls);

    if (mode == GhostMode::LEAVING ||
        mode == GhostMode::ENTERING)
    {
        filteredWalls.erase(std::remove_if(filteredWalls.begin(), filteredWalls.end(),
                                           [](const Wall &wall)
                                           {
                                               return wall.type == CellType::GHOST_DOOR;
                                           }),
                            filteredWalls.end());
    }

    return filteredWalls;
}

float Ghost::GetMovementSpeed(GhostMode mode, const sf::Vector2f &position, float maxX)
{
    float multiplier = 1.0f; // Normal

    if (mode == GhostMode::FRIGHTENED)
    {
        multiplier = 0.5f; // Half speed
    }
    else if (mode == GhostMode::SPAWN ||
             mode == GhostMode::ENTERING)
    {
        multiplier = 1.5f; // 50% increase
    }
    else if (position.y == Constants::GRID_OFFSET_Y + 14.0f &&
             (position.x < 6.0f || position.x > maxX - 6.0f))
    {
        multiplier = 0.5f; // Half speed in tunnels
    }

    return speed * multiplier;
}

DirectionOption Ghost::PickDirection(const std::vector<DirectionOption> &directions, Direction currentDirection)
{
    if (directions.size() == 1)
    {
        return directions.front();
    }

    return *std::find_if(directions.cbegin(), directions.cend(),
                         [&currentDirection](const DirectionOption opt)
                         {
                             return opt.direction == currentDirection;
                         });
}
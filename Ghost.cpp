#include "pch.h"

#include <cstdlib>

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

    // TODO
    // - only determine direction at intersections
    // - at non intersections continue until a wall is hit, then make the only available turn
    Direction newDirection = justExitedTheHouse
                                 ? Direction::LEFT
                                 : DetermineDirection(deltaTime, walls, calcMode, movementSpeed, lastKnownDirection, sprite, targetPosition, forceReverseDirection, collisionOffset);

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

Direction Ghost::DetermineDirection(float deltaTime, const std::vector<Wall> &walls, GhostMode mode, float movementSpeed, Direction lastMovedDirection, sf::Sprite ghost, sf::Vector2f targetPosition, bool forceReverseDirection, sf::Vector2f &collisionOffset)
{
    // Possible directions
    std::vector<DirectionOption> validDirectionsForFrightened;
    std::vector<Direction> directions = {UP, LEFT, DOWN, RIGHT};
    ExcludeDirections(mode, lastMovedDirection, forceReverseDirection, directions);

    float epsilon = 1e-5;
    float minDistance = std::numeric_limits<float>::max();
    Direction selectedDirection = lastMovedDirection;
    std::unordered_map<Direction, int> priorityMap = {{UP, 1}, {LEFT, 2}, {DOWN, 3}, {RIGHT, 4}};

    sf::Vector2f currentPosition = ghost.getPosition();
    sf::Vector2f localCollisionOffset;

    std::vector<Wall> filteredWalls(walls);
    if (mode == GhostMode::LEAVING ||
        mode == GhostMode::ENTERING)
    {
        filteredWalls.erase(std::remove_if(filteredWalls.begin(), filteredWalls.end(),
                                           [](const Wall &wall)
                                           {
                                               return Ghost::IsGhostDoor(wall);
                                           }),
                            filteredWalls.end());
    }

    bool isFrightened = mode == GhostMode::FRIGHTENED;
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

        if (!isFrightened)
        {
            float distance = CalculateDistance(nextPosition, targetPosition);

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
        else
        {
            validDirectionsForFrightened.push_back({direction, localCollisionOffset});
        }
    }

    // Frightened ghosts: Select random direction from the valid options
    if (isFrightened)
    {
        int randomIndex = std::rand() % validDirectionsForFrightened.size();
        DirectionOption option = validDirectionsForFrightened[randomIndex];
        selectedDirection = option.direction;
        collisionOffset = option.offset;
    }

    // TODO DEBUGGING REMOVE LATER
    // if (personality == GhostPersonality::BLINKY &&
    //     lastKnownDirection != selectedDirection)
    // {
    //     std::cout << "Changed Direction! forcedReverse= "
    //               << (forceReverseDirection
    //                       ? "true"
    //                       : "false")
    //               << ", previousDirection= "
    //               << EnumUtils::DirectionToString(lastKnownDirection)
    //               << ", selectedDirection= "
    //               << EnumUtils::DirectionToString(selectedDirection)
    //               << std::endl;

    //     auto reverseDirection = Constants::REVERSE_DIRECTION_MAP.find(lastMovedDirection);
    //     if (reverseDirection != Constants::REVERSE_DIRECTION_MAP.end() &&
    //         selectedDirection == reverseDirection->second)
    //     {
    //         std::cout << "Direction Was Reversed! forcedReverse= "
    //                   << (forceReverseDirection
    //                           ? "true"
    //                           : "false")
    //                   << std::endl;
    //     }
    // }

    return selectedDirection;
}

void Ghost::ExcludeDirections(const GhostMode &mode, const Direction &lastMovedDirection, bool forceReverseDirection, std::vector<Direction> &directions)
{
    if (mode == GhostMode::HOUSED)
    {
        directions.erase(std::remove(directions.begin(), directions.end(), Direction::LEFT), directions.end());
        directions.erase(std::remove(directions.begin(), directions.end(), Direction::RIGHT), directions.end());
    }
    else if (mode == GhostMode::LEAVING)
    {
        directions.erase(std::remove(directions.begin(), directions.end(), Direction::DOWN), directions.end());
    }
    else if (mode == GhostMode::ENTERING)
    {
        directions.erase(std::remove(directions.begin(), directions.end(), Direction::UP), directions.end());
    }
    else
    {
        // Exclude or Force reverse direction
        auto reverseDirection = Constants::REVERSE_DIRECTION_MAP.find(lastMovedDirection);

        directions.erase(std::remove_if(directions.begin(),
                                        directions.end(),
                                        [&reverseDirection, forceReverseDirection](Direction &direction)
                                        {
                                            if (forceReverseDirection)
                                            {
                                                return direction != reverseDirection->second;
                                            }

                                            return direction == reverseDirection->second;
                                        }),
                         directions.end());
    }
}

bool Ghost::IsGhostDoor(const Wall &wall)
{
    return wall.type == CellType::GHOST_DOOR;
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
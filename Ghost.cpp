#include "iostream"

#include "include/CellType.h"
#include "include/Constants.h"
#include "include/Ghost.h"
#include "include/GhostModeController.h"
#include "include/GhostMovement.h"
#include "include/Player.h"
#include "include/Wall.h"

#include "include/EnumUtils.h"

Ghost::Ghost(sf::Texture &sharedTexture, float spriteSize, int spriteSheetColumnIndex)
    : texture(sharedTexture), animation(&sharedTexture, 0.1f, spriteSheetColumnIndex)
{
    sprite.setTexture(sharedTexture);
    sprite.setTextureRect(sf::IntRect(spriteSheetColumnIndex * 32, 64, 32, 32));

    sf::FloatRect bounds = sprite.getLocalBounds();
    float centreX = bounds.width / 2.0f;
    float centreY = bounds.height / 2.0f;
    sprite.setOrigin(centreX, centreY);

    float scaleX = spriteSize / bounds.width;
    float scaleY = spriteSize / bounds.height;
    sprite.setScale(scaleX, scaleY);

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
    bool forceReverseDirection = mode->CheckForcedReverseQueue(personality);
    bool justExitedTheHouse = lastKnownMode == GhostMode::LEAVING &&
                              currentMode != GhostMode::LEAVING;

    sf::Vector2f currentPosition = sprite.getPosition();
    sf::Vector2f collisionOffset;

    sf::Vector2f targetPosition = GhostMovement::GetTargetTile(personality, currentMode, walls, ghosts, player, deltaTime);
    Direction newDirection = justExitedTheHouse
                                 ? Direction::LEFT
                                 : DetermineDirection(deltaTime, walls, currentMode, lastKnownDirection, sprite, targetPosition, forceReverseDirection, collisionOffset);

    // Set position and offset for determined direction
    sf::Vector2f newPosition = currentPosition + GhostMovement::GetDirectionVector(newDirection, speed, deltaTime);
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
    animation.Update(newDirection, deltaTime);
    sprite.setTextureRect(animation.textureRect);

    // Update to new current values
    lastKnownDirection = newDirection;
    lastKnownMode = currentMode;
}

void Ghost::SetPosition(float x, float y)
{
    sprite.setPosition(x, y);
}

void Ghost::Draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}

// Private

float Ghost::CalculateDistance(sf::Vector2f a, sf::Vector2f b)
{
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

Direction Ghost::DetermineDirection(float deltaTime, const std::vector<Wall> &walls, GhostMode mode, Direction lastMovedDirection, sf::Sprite ghost, sf::Vector2f targetPosition, bool forceReverseDirection, sf::Vector2f &collisionOffset)
{
    // Possible directions
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
        mode == GhostMode::SPAWN)
    {
        filteredWalls.erase(std::remove_if(filteredWalls.begin(), filteredWalls.end(),
                                           [](const Wall &wall)
                                           {
                                               return Ghost::IsGhostDoor(wall);
                                           }),
                            filteredWalls.end());
    }

    for (Direction direction : directions)
    {
        sf::Vector2f velocity = GhostMovement::GetDirectionVector(direction, speed, deltaTime);
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

    // TODO DEBUGGING REMOVE LATER
    // if (personality == GhostPersonality::BLINKY &&
    //     lastKnownDirection != selectedDirection)
    // {
    //     std::cout << "Change Direction !! "
    //               << "previousDirection = "
    //               << EnumUtils::DirectionToString(lastKnownDirection)
    //               << ", selectedDirection = "
    //               << EnumUtils::DirectionToString(selectedDirection)
    //               << std::endl;
    // }

    return selectedDirection;
}

void Ghost::ExcludeDirections(const GhostMode &mode, const Direction &lastMovedDirection, bool forceReverseDirection, std::vector<Direction> &directions)
{
    if (mode == GhostMode::HOUSED)
    {
        directions.erase(std::remove(directions.begin(), directions.end(), Direction::UP), directions.end());
        directions.erase(std::remove(directions.begin(), directions.end(), Direction::DOWN), directions.end());
    }
    else if (mode == GhostMode::LEAVING)
    {
        directions.erase(std::remove(directions.begin(), directions.end(), Direction::DOWN), directions.end());
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
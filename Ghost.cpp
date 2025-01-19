#include "iostream"

#include "include/Ghost.h"
#include "include/GhostModeController.h"
#include "include/GhostMovement.h"
#include "include/Player.h"

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

    currentDirection = Direction::NONE;
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

void Ghost::Update(float deltaTime, const std::vector<sf::RectangleShape> &walls, const std::vector<Ghost> &ghosts, const Player &player, float minX, float maxX)
{
    GhostMode currentMode = mode->GetMode();
    sf::Vector2f currentPosition = sprite.getPosition();
    sf::Vector2f collisionOffset;

    sf::Vector2f targetPosition = GhostMovement::GetTargetTile(personality, currentMode, walls, ghosts, player, deltaTime);
    Direction newDirection = DetermineDirection(deltaTime, walls, currentDirection, sprite, targetPosition, collisionOffset);
    sf::Vector2f newPosition = currentPosition + GhostMovement::GetDirectionVector(newDirection, speed, deltaTime);

    // Set position and offset for determined direction
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
    currentDirection = newDirection;
}

void Ghost::SetPosition(float x, float y)
{
    sprite.setPosition(x, y);
}

void Ghost::Draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}

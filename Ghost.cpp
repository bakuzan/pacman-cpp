#include "iostream"

#include "include/Ghost.h"
#include "include/GhostModeController.h"
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
    nextDirection = Direction::LEFT;
    mode = GhostModeController::GetInstance();

    std::cout << "SpriteSheetColumnIndex: " << spriteSheetColumnIndex << "\n";
    std::cout << "Texture Rect: " << sprite.getTextureRect().left << ", "
              << sprite.getTextureRect().top << ", "
              << sprite.getTextureRect().width << ", "
              << sprite.getTextureRect().height << "\n";
}

Ghost::~Ghost()
{
    // Destructor
}

void Ghost::Update(float deltaTime, const std::vector<sf::RectangleShape> &walls, const Player &player, float minX, float maxX)
{
    Direction movingDirection = nextDirection;
    GhostMode currentMode = mode->GetMode();
    targetTile = GetTargetTile(currentMode, player);

    // TODO
    // Move towards target tile (not allowed to turn back) check collisions
    // Set nextDirection

    animation.Update(movingDirection, deltaTime);
}

void Ghost::SetPosition(float x, float y)
{
    sprite.setPosition(x, y);
}

void Ghost::Draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}

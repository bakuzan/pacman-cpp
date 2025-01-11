#include "iostream"

#include "include/Ghost.h"

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

void Ghost::Update(float deltaTime, const std::vector<sf::RectangleShape> &walls, float minX, float maxX)
{
    Direction movingDirection = Direction::UP;

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

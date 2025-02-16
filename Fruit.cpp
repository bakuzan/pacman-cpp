#include "include/Constants.h"
#include "include/Fruit.h"
#include "include/SFMLUtils.h"

Fruit::Fruit(sf::Texture &sharedTexture, float spriteSize, float displayThreshold)
    : texture(sharedTexture), displayThreshold(displayThreshold), triggeredTime(0.0f), triggered(false), show(false), points(100)
{
    sprite.setTexture(sharedTexture);
    SetTextureRect(1);
    SFMLUtils::CenterOriginAndScale(sprite, spriteSize);
    sprite.setPosition(14.0f, Constants::GRID_OFFSET_Y + 17.0f);

    SetPoints(1);
}

Fruit::~Fruit()
{
    // Destructor
}

float Fruit::GetDisplayThreshold()
{
    return displayThreshold;
}

float Fruit::GetDisplayTime()
{
    return triggeredTime;
}

bool Fruit::IsShown()
{
    return show;
}

int Fruit::GetPoints()
{
    return points;
}

sf::Vector2f Fruit::GetPosition()
{
    return sprite.getPosition();
}

void Fruit::Show(int level, float startTime)
{
    if (!triggered)
    {
        triggered = true;
        triggeredTime = startTime;
        show = true;
        SetTextureRect(level);
        SetPoints(level);
    }
}

void Fruit::Hide()
{
    show = false;
}

void Fruit::Draw(sf::RenderWindow &window)
{
    if (show)
    {
        window.draw(sprite);
    }
}

void Fruit::Reset(int level)
{
    triggeredTime = 0.0f;
    triggered = false;
    show = false;
    SetTextureRect(level);
    SetPoints(level);
}

// Private
void Fruit::SetTextureRect(int level)
{
    int size = 32;
    switch (level)
    {
    case 1:
        sprite.setTextureRect(sf::IntRect(8 * size, 4 * size, size, size));
        break;
    case 2:
        sprite.setTextureRect(sf::IntRect(8 * size, 5 * size, size, size));
    case 3:
    case 4:
        sprite.setTextureRect(sf::IntRect(9 * size, 4 * size, size, size));
        break;
    case 5:
    case 6:
        sprite.setTextureRect(sf::IntRect(9 * size, 5 * size, size, size));
        break;
    case 7:
    case 8:
        sprite.setTextureRect(sf::IntRect(10 * size, 4 * size, size, size));
    default:
        sprite.setTextureRect(sf::IntRect(10 * size, 5 * size, size, size));
        break;
    }
}

void Fruit::SetPoints(int level)
{
    switch (level)
    {
    case 1:
        points = 100;
        break;
    case 2:
        points = 300;
    case 3:
    case 4:
        points = 500;
        break;
    case 5:
    case 6:
        points = 700;
        break;
    case 7:
    case 8:
        points = 1000;
    default:
        points = 2000;
        break;
    }
}
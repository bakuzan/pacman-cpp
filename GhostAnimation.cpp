
#include "include/GhostAnimation.h"

GhostAnimation::GhostAnimation(sf::Texture *texture, float switchTime, float columnIndex)
    : columnIndex(columnIndex), defaultRect(columnIndex * 32, 64, 32, 32)
{
    this->switchTime = switchTime;
    this->totalTime = 0.0f;

    currentImage.x = columnIndex;
    currentImage.y = 2;

    textureRect = defaultRect;
}

GhostAnimation::~GhostAnimation()
{
    // Destructor
}

void GhostAnimation::Update(Direction direction, float deltaTime)
{
    switch (direction)
    {
    case Direction::DOWN:
        currentImage.y = 3;
        break;
    case Direction::LEFT:
        currentImage.y = 4;
        break;
    case Direction::RIGHT:
        currentImage.y = 5;
        break;
    case Direction::UP:
    default:
        currentImage.y = 2;
        break;
    }

    textureRect.top = currentImage.y * textureRect.height;
    textureRect.left = currentImage.x * textureRect.width;
}

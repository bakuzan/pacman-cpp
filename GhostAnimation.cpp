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

void GhostAnimation::Update(GhostMode mode, Direction direction, float frightTimeElapsed)
{
    if (mode != GhostMode::FRIGHTENED)
    {
        switch (direction)
        {
        case Direction::DOWN:
            currentImage.x = columnIndex;
            currentImage.y = 3;
            break;
        case Direction::LEFT:
            currentImage.x = columnIndex;
            currentImage.y = 4;
            break;
        case Direction::RIGHT:
            currentImage.x = columnIndex;
            currentImage.y = 5;
            break;
        case Direction::UP:
        default:
            currentImage.x = columnIndex;
            currentImage.y = 2;
            break;
        }
    }
    else
    {
        currentImage.x = 5;
        currentImage.y = frightTimeElapsed < 4.0f
                             ? 2
                         : currentImage.y != 2
                             ? 2
                             : 3;
    }

    textureRect.top = currentImage.y * textureRect.height;
    textureRect.left = currentImage.x * textureRect.width;
}

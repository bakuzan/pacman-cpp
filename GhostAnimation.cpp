#include "include/GhostAnimation.h"

GhostAnimation::GhostAnimation(sf::Texture *texture, float blinkInterval, float columnIndex)
    : blinkInterval(blinkInterval), columnIndex(columnIndex), defaultRect(columnIndex * 32, 64, 32, 32)
{
    Init();
}

GhostAnimation::~GhostAnimation()
{
    // Destructor
}

void GhostAnimation::Reset()
{
    Init();
}

void GhostAnimation::Update(GhostMode mode, Direction direction, float deltaTime, float frightTimeElapsed)
{
    if (mode == GhostMode::FRIGHTENED)
    {
        UpdateFrightenedSprite(deltaTime, frightTimeElapsed);
    }
    else
    {
        blinkTimeElapsed = 0.0f; // Ensure is 0 for next frightened run.

        currentImage.x = mode != GhostMode::SPAWN
                             ? columnIndex
                             : 4;

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
    }

    textureRect.top = currentImage.y * textureRect.height;
    textureRect.left = currentImage.x * textureRect.width;
}

// Private
void GhostAnimation::Init()
{
    this->blinkTimeElapsed = 0.0f;

    currentImage.x = columnIndex;
    currentImage.y = 2;

    textureRect = defaultRect;
}

void GhostAnimation::UpdateFrightenedSprite(float deltaTime, float frightTimeElapsed)
{
    currentImage.x = 5;

    if (frightTimeElapsed < (4.0f - blinkInterval))
    {
        currentImage.y = 2;
    }
    else
    {
        blinkTimeElapsed += deltaTime;

        if (blinkTimeElapsed >= blinkInterval)
        {
            blinkTimeElapsed = 0.0f;
            currentImage.y = (currentImage.y == 2) ? 3 : 2;
        }
    }
}

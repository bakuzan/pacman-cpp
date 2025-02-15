#include "pch.h"

#include "include/PlayerAnimation.h"
#include "include/Direction.h"

PlayerAnimation::PlayerAnimation(sf::Texture *texture, float switchTime)
    : switchTime(switchTime), defaultRect(128, 0, 32, 32)
{
    Init();
}

PlayerAnimation::~PlayerAnimation()
{
}

void PlayerAnimation::Update(Direction direction, float deltaTime)
{
    if (direction == Direction::NONE)
    {
        return;
    }

    currentImage.x = static_cast<unsigned int>(direction);
    totalTime += deltaTime;

    if (totalTime >= switchTime)
    {
        totalTime -= switchTime;

        if (returningToDefault)
        {
            textureRect = defaultRect;
            returningToDefault = false;
        }
        else
        {
            if (textureRect.top == defaultRect.top &&
                textureRect.left == defaultRect.left)
            {
                currentImage.y = 0;
            }
            else if (currentImage.y == 0)
            {
                currentImage.y = 1;
            }
            else if (currentImage.y == 1)
            {
                currentImage.y = 0;
                returningToDefault = true;
            }

            textureRect.top = currentImage.y * textureRect.height;
            textureRect.left = currentImage.x * textureRect.width;
        }
    }
}

void PlayerAnimation::Reset()
{
    Init();
}

bool PlayerAnimation::NextDeathFrame()
{
    if (deathFrame > 10)
    {
        deathFrame = 0;
    }

    currentImage.x = deathFrame;

    textureRect.top = 6 * textureRect.height;
    textureRect.left = currentImage.x * textureRect.width;

    deathFrame++;
    return deathFrame <= 10;
}

// Private
void PlayerAnimation::Init()
{
    this->totalTime = 0.0f;
    this->returningToDefault = false;
    this->deathFrame = 0;

    currentImage.x = static_cast<unsigned int>(Direction::NONE);
    currentImage.y = 0;

    textureRect = defaultRect;
}
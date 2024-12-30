#include "include/Animation.h"
#include "include/Direction.h"

Animation::Animation(sf::Texture *texture, float switchTime)
    : defaultRect(128, 0, 32, 32)
{
    this->switchTime = switchTime;
    this->totalTime = 0.0f;
    this->returningToDefault = false;

    currentImage.x = static_cast<unsigned int>(Direction::NONE);
    currentImage.y = 0;

    textureRect = defaultRect;
}

Animation::~Animation()
{
}

void Animation::Update(Direction direction, float deltaTime)
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

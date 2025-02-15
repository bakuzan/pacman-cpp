#include "pch.h"

#include "include/SFMLUtils.h"

namespace SFMLUtils
{
    void CenterOriginAndScale(sf::Sprite &sprite, float spriteSize)
    {
        sf::FloatRect bounds = sprite.getLocalBounds();
        float centreX = bounds.width / 2.0f;
        float centreY = bounds.height / 2.0f;
        sprite.setOrigin(centreX, centreY);

        float scaleX = spriteSize / bounds.width;
        float scaleY = spriteSize / bounds.height;
        sprite.setScale(scaleX, scaleY);
    }
};

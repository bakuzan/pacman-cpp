#include "pch.h"

#include "include/SFMLUtils.h"

namespace SFMLUtils
{
    void SetWindowIcon(sf::RenderWindow &window, sf::Image spritesheet)
    {
        sf::IntRect iconRegion(32, 0, 32, 32); // first row, second column

        // Create an image for the icon
        sf::Image icon;
        icon.create(iconRegion.width, iconRegion.height);
        icon.copy(spritesheet, 0, 0, iconRegion);

        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    };

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

#ifndef SFMLUTILS_H
#define SFMLUTILS_H

#include <SFML/Graphics.hpp>

namespace SFMLUtils
{
    void SetWindowIcon(sf::RenderWindow &window, sf::Image spritesheet);
    void CenterOriginAndScale(sf::Sprite &sprite, float spriteSize);
};

#endif // SFMLUTILS_H

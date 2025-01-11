#include <SFML/Graphics.hpp>

#include "GhostAnimation.h"

#ifndef GHOST_H
#define GHOST_H

class Ghost
{
public:
    Ghost(sf::Texture &sharedTexture, float spriteSize, int spriteSheetColumnIndex);
    ~Ghost();

    sf::Vector2f GetPosition() { return sprite.getPosition(); };

    void Draw(sf::RenderWindow &window);
    void SetPosition(float x, float y);
    void Update(float deltaTime, const std::vector<sf::RectangleShape> &walls, float minX, float maxX);

private:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f velocity;
    GhostAnimation animation;
};

#endif // GHOST_H

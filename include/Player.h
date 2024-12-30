#include <SFML/Graphics.hpp>

#include "Animation.h"
#include "Direction.h"

#ifndef PLAYER_H
#define PLAYER_H

class Player
{
public:
    Player(sf::Image &spritesheet, float spriteSize);
    ~Player();

    void Draw(sf::RenderWindow &window);
    void SetDirection(Direction newDirection);
    void SetPosition(float x, float y);
    void Update(float deltaTime);

private:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f velocity;
    Direction direction;
    Animation animation;
};

#endif // PLAYER_H

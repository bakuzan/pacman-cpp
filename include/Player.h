#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

#include "PlayerAnimation.h"
#include "Direction.h"
#include "Wall.h"

class Player
{
public:
    Player(sf::Texture &sharedTexture, float spriteSize);
    ~Player();

    sf::Vector2f GetPosition() const;
    Direction GetDirection() const;
    float GetSpeed() const;

    void Draw(sf::RenderWindow &window);
    void SetDirection(Direction newDirection);
    void SetSpawnPosition(float x, float y);
    void Update(Direction newDirection, float deltaTime, const std::vector<Wall> &walls, float minX, float maxX);
    bool Dying();
    void Reset();

private:
    sf::Texture &texture;
    sf::Sprite sprite;
    PlayerAnimation animation;

    sf::Vector2f spawnPosition;
    float speed;

    sf::Vector2f velocity;
    Direction direction;

private:
    void UpdateVelocity(Direction newDirection, float deltaTime);
};

#endif // PLAYER_H

#include <SFML/Graphics.hpp>

#include "Animation.h"
#include "Direction.h"
#include "Wall.h"

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
    void Update(float deltaTime, const std::vector<Wall> &walls);

private:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f velocity;
    Direction direction;
    Animation animation;

private:
    bool CheckTileCollision(sf::Vector2f newPosition, const std::vector<Wall> &walls)
    {
        int gridX = static_cast<int>(newPosition.x);
        int gridY = static_cast<int>(newPosition.y);

        for (const auto &wall : walls)
        {
            int wallX = static_cast<int>(wall.x);
            int wallY = static_cast<int>(wall.y);

            if (wallX == gridX &&
                wallY == gridY)
            {
                return true;
            }
        }

        return false;
    }
};

#endif // PLAYER_H

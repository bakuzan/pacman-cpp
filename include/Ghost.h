#include <SFML/Graphics.hpp>

#include "GhostAnimation.h"
#include "GhostMode.h"
#include "GhostPersonality.h"
#include "GhostModeController.h"
#include "Player.h"

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
    void Update(float deltaTime, const std::vector<sf::RectangleShape> &walls, const Player &player, float minX, float maxX);

private:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f velocity;
    GhostAnimation animation;

    GhostPersonality personality;
    float speed;

    GhostModeController *mode;

    Direction currentDirection;
    Direction nextDirection;
    sf::Vector2f targetTile;

private:
    sf::Vector2f GetTargetTile(GhostMode currentMode, const Player &player)
    {
        switch (currentMode)
        {
        case GhostMode::SCATTER:
            return sf::Vector2f();
        case GhostMode::CHASE:
            return player.GetPosition();
        default:
            return sf::Vector2f();
        }
    }
    void UpdateVelocity(Direction newDirection, float deltaTime)
    {
        velocity.x = 0.0f;
        velocity.y = 0.0f;

        switch (newDirection)
        {
        case Direction::LEFT:
            velocity.x -= speed * deltaTime;
            break;
        case Direction::UP:
            velocity.y -= speed * deltaTime;
            break;
        case Direction::RIGHT:
            velocity.x += speed * deltaTime;
            break;
        case Direction::DOWN:
            velocity.y += speed * deltaTime;
            break;
        default:
            // No movement, do nothing
            break;
        }
    }
};

#endif // GHOST_H

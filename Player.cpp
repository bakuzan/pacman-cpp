#include <SFML/Graphics.hpp>

#include "include/CellType.h"
#include "include/Collider.h"
#include "include/Direction.h"
#include "include/Player.h"
#include "include/Wall.h"

Player::Player(sf::Texture &sharedTexture, float spriteSize)
    : texture(sharedTexture), animation(&sharedTexture, 0.1f), speed(5.0f)
{
    this->direction = Direction::NONE;

    sprite.setTexture(sharedTexture);
    sprite.setTextureRect(sf::IntRect(128, 0, 32, 32));

    sf::FloatRect bounds = sprite.getLocalBounds();
    float centreX = bounds.width / 2.0f;
    float centreY = bounds.height / 2.0f;
    sprite.setOrigin(centreX, centreY);

    float scaleX = spriteSize / bounds.width;
    float scaleY = spriteSize / bounds.height;
    sprite.setScale(scaleX, scaleY);
}

Player::~Player()
{
    // Destructor
}

sf::Vector2f Player::GetPosition() const
{
    return sprite.getPosition();
}

Direction Player::GetDirection() const
{
    return this->direction;
}

float Player::GetSpeed() const
{
    return this->speed;
}

void Player::Update(Direction newDirection, float deltaTime, const std::vector<Wall> &walls, float minX, float maxX)
{
    // No new direction means stay the course!
    newDirection = newDirection == Direction::NONE
                       ? this->direction
                       : newDirection;

    bool canMove = true;
    sf::Vector2f collisionOffset;
    sf::Vector2f originalPosition = sprite.getPosition();
    UpdateVelocity(newDirection, deltaTime);

    sf::Vector2f newPosition = originalPosition + velocity;
    sprite.setPosition(newPosition);

    if (direction != newDirection &&
        Collider::CheckTileCollision(sprite, walls, collisionOffset))
    {
        canMove = false;
        sprite.setPosition(originalPosition);
    }

    // If you cant move in the new direction, try to keep moving in your current one
    if ((!canMove || direction == newDirection) &&
        direction != Direction::NONE)
    {
        UpdateVelocity(direction, deltaTime);

        newPosition = originalPosition + velocity;
        sprite.setPosition(newPosition);

        if (Collider::CheckTileCollision(sprite, walls, collisionOffset))
        {
            canMove = false;
            sprite.move(-collisionOffset);
        }
        else
        {
            // Make newDirection equal to current direction if that movement is good.
            canMove = true;
            newDirection = this->direction;
        }
    }

    if (canMove)
    {
        this->direction = newDirection;

        // Do we need to "teleport"?
        if (newPosition.x < minX ||
            newPosition.x > maxX)
        {
            newPosition.x = newPosition.x < minX
                                ? maxX
                                : minX;
            sprite.setPosition(newPosition);
        }

        animation.Update(this->direction, deltaTime);
        sprite.setTextureRect(animation.textureRect);
    }
    else if (direction == newDirection)
    {
        this->direction = Direction::NONE;
        this->velocity.x = 0.0f;
        this->velocity.y = 0.0f;
    }
}

void Player::SetDirection(Direction newDirection)
{
    direction = newDirection;
}

void Player::SetSpawnPosition(float x, float y)
{
    sprite.setPosition(x, y);
    spawnPosition = sf::Vector2f(x, y);
}

void Player::Draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}

// Private

void Player::UpdateVelocity(Direction newDirection, float deltaTime)
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
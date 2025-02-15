#include "pch.h"

#include "include/CellType.h"
#include "include/Collider.h"
#include "include/Direction.h"
#include "include/Player.h"
#include "include/SFMLUtils.h"
#include "include/Wall.h"

Player::Player(sf::Texture &sharedTexture, float spriteSize)
    : texture(sharedTexture), animation(&sharedTexture, 0.1f), speed(5.0f)
{
    this->direction = Direction::NONE;

    sprite.setTexture(sharedTexture);
    sprite.setTextureRect(animation.defaultRect);
    SFMLUtils::CenterOriginAndScale(sprite, spriteSize);
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

bool Player::Dying()
{
    bool hasMoreFrames = animation.NextDeathFrame();
    sprite.setTextureRect(animation.textureRect);
    return hasMoreFrames;
}

void Player::Reset()
{
    direction = Direction::NONE;
    animation.Reset();
    sprite.setTextureRect(animation.defaultRect);
    sprite.setPosition(spawnPosition);
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
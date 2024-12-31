#include <SFML/Graphics.hpp>

#include "include/CellType.h"
#include "include/Direction.h"
#include "include/Player.h"
#include "include/Wall.h"

Player::Player(sf::Image &spritesheet, float spriteSize)
    : animation(&texture, 0.1f)
{
    this->direction = Direction::NONE;

    texture.loadFromImage(spritesheet);

    sprite.setTexture(texture);
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

void Player::Update(float deltaTime, const std::vector<Wall> &walls)
{
    Direction newDirection = this->direction;
    float speed = 5.0f;
    velocity.x = 0.0f;
    velocity.y = 0.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        newDirection = Direction::LEFT;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        newDirection = Direction::RIGHT;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        newDirection = Direction::UP;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        newDirection = Direction::DOWN;
    }

    switch (newDirection)
    {
    case Direction::LEFT:
        velocity.x -= speed;
        break;
    case Direction::UP:
        velocity.y -= speed;
        break;
    case Direction::RIGHT:
        velocity.x += speed;
        break;
    case Direction::DOWN:
        velocity.y += speed;
        break;
    default:
        // No movement, do nothing
        break;
    }

    sf::Vector2f position = sprite.getPosition();
    sf::Vector2f newPosition = position + velocity * deltaTime;

    if (!CheckTileCollision(newPosition, walls))
    {
        this->direction = newDirection;
        animation.Update(direction, deltaTime);
        sprite.setTextureRect(animation.textureRect);
        sprite.move(velocity * deltaTime);
    }
    else if (direction == newDirection)
    {
        this->direction = Direction::NONE;
    }
}

void Player::SetDirection(Direction newDirection)
{
    direction = newDirection;
}

void Player::SetPosition(float x, float y)
{
    sprite.setPosition(x, y);
}

void Player::Draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}

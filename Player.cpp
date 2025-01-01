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

    float scaleX = (spriteSize * 0.95f) / bounds.width;
    float scaleY = (spriteSize * 0.95f) / bounds.height;
    sprite.setScale(scaleX, scaleY);
}

Player::~Player()
{
    // Destructor
}

void Player::Update(float deltaTime, const std::vector<sf::RectangleShape> &walls)
{
    Direction newDirection = this->direction;

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

    bool canMove = CanMove(newDirection, deltaTime, walls);

    // If you cant move in the new direction, try to keep moving in your current one
    if (!canMove &&
        direction != newDirection &&
        direction != Direction::NONE)
    {
        newDirection = this->direction; // Put newDirection back to current direction;
        canMove = CanMove(newDirection, deltaTime, walls);
    }

    if (canMove)
    {
        this->direction = newDirection;

        animation.Update(this->direction, deltaTime);
        sprite.setTextureRect(animation.textureRect);
        sprite.move(velocity * deltaTime);
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

void Player::SetPosition(float x, float y)
{
    sprite.setPosition(x, y);
}

void Player::Draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}

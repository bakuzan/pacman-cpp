#include <SFML/Graphics.hpp>

#include "include/Direction.h"
#include "include/Player.h"

Player::Player(sf::Image &spritesheet, float spriteSize)
{
    this->direction = Direction::LEFT;

    sf::Texture pacmanTexture;
    pacmanTexture.loadFromImage(spritesheet);

    sprite.setTexture(pacmanTexture);
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

void Player::Update(float deltaTime)
{
    float speed = 5.0f;
    velocity.x = 0.0f;
    velocity.y = 0.0f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        direction = Direction::LEFT;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        direction = Direction::RIGHT;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        direction = Direction::UP;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        direction = Direction::DOWN;
    }

    switch (direction)
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

    // TODO update sprite so is animated
    sprite.move(velocity * deltaTime);
}

void Player::SetPosition(float x, float y)
{
    sprite.setPosition(x, y);
}

void Player::Draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}
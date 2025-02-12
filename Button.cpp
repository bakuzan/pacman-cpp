#include "include/Button.h"
#include "include/Constants.h"

Button::Button(sf::Font &font, float x, float y, float width, float height, const std::string &text, std::function<void()> onClick)
    : font(font), onClick(onClick)
{
    // Create Button shape
    shape.setPosition(x, y);
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(sf::Color(31, 31, 31));
    shape.setOutlineThickness(0.5f);
    shape.setOutlineColor(sf::Color(53, 56, 57));
    shape.setOrigin(width / 2.0f, height / 2.0f);

    // Create text label
    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(24);
    label.setScale(height / Constants::GRID_HEIGHT, height / Constants::GRID_HEIGHT);
    label.setFillColor(sf::Color(242, 243, 244));

    // Centre text in shape
    sf::FloatRect labelBounds = label.getLocalBounds();
    label.setOrigin(labelBounds.left + labelBounds.width / 2.0f,
                    labelBounds.top + labelBounds.height / 2.0f);
    label.setPosition(x + width / 2.0f, y + height / 2.0f);
}

Button::~Button()
{
    // Destructor
}

void Button::Draw(sf::RenderWindow &window)
{
    window.draw(shape);
    window.draw(label);
}

void Button::HandleEvent(const sf::Event &event, sf::Vector2i mousePos)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (shape.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
        {
            onClick();
        }
    }

    if (shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
    {
        shape.setOutlineColor(sf::Color::White);
    }
    else
    {
        shape.setOutlineColor(sf::Color(53, 56, 57));
    }
}
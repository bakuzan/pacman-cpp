#include "include/Button.h"
#include "include/Constants.h"

Button::Button(sf::Font &font, float x, float y, float width, float height, const std::string &text, std::function<void()> onClick)
    : font(font), onClick(onClick), defaultColour(sf::Color(53, 56, 57)), hoverColour(sf::Color::White)
{
    // Create Button shape
    shape.setPosition(x, y);
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(sf::Color(31, 31, 31));
    shape.setOutlineThickness(0.5f);
    shape.setOutlineColor(defaultColour);
    shape.setOrigin(width / 2.0f, height / 2.0f);

    // Create text label
    label.setPosition(x, y);
    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(24);
    label.setScale(height / Constants::GRID_HEIGHT, height / Constants::GRID_HEIGHT);
    label.setFillColor(defaultColour);

    // Centre text in shape
    sf::FloatRect labelBounds = label.getLocalBounds();
    label.setOrigin(labelBounds.width / 2.0f,
                    labelBounds.height / 2.0f);
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

void Button::HandleEvent(const sf::Event &evnt, sf::Vector2f mousePos)
{
    if (evnt.type == sf::Event::MouseButtonPressed)
    {
        if (shape.getGlobalBounds().contains(mousePos))
        {
            onClick();
        }
    }

    // Check hover state
    if (shape.getGlobalBounds().contains(mousePos))
    {
        shape.setOutlineColor(hoverColour);
        label.setFillColor(hoverColour);
    }
    else
    {
        shape.setOutlineColor(defaultColour);
        label.setFillColor(defaultColour);
    }
}
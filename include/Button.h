#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>
#include <functional>

class Button
{
public:
    Button(sf::Font &font, float x, float y, float width, float height, const std::string &text, std::function<void()> onClick);
    ~Button();

    void Draw(sf::RenderWindow &window);
    void HandleEvent(const sf::Event &evnt, sf::Vector2f mousePos);

private:
    sf::RectangleShape shape;
    sf::Text label;
    sf::Font font;
    std::function<void()> onClick;

    sf::Color defaultColour;
    sf::Color hoverColour;
};

#endif // BUTTON_H

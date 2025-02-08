#ifndef TEXTMANAGER_H
#define TEXTMANAGER_H

#include <SFML/Graphics.hpp>

class TextManager
{
public:
    TextManager(sf::Font &font);
    ~TextManager();

    void UpdateScoreText(int score);
    void DrawPreGame(sf::RenderWindow &window);
    void Draw(sf::RenderWindow &window);

private:
    sf::Font &font;
    sf::Text readyText;
    sf::Text scoreLabelText;
    sf::Text scoreText;

private:
    sf::Text GetReadyText();
    sf::Text GetScoreLabelText();
    sf::Text GetScoreText();
    void SetScale(sf::Text &text);

    std::string Pad(std::string str, int width, char c);
};

#endif // TEXTMANAGER_H

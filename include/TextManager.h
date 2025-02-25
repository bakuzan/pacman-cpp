#ifndef TEXTMANAGER_H
#define TEXTMANAGER_H

#include <SFML/Graphics.hpp>

#include "GameStatus.h"

class TextManager
{
public:
    TextManager(sf::Font &font);
    ~TextManager();

    void DrawMenuTitle(sf::RenderWindow &window);

    void UpdateScoreText(int score);
    void DrawPreGame(sf::RenderWindow &window);
    void Draw(sf::RenderWindow &window, const GameStatus &gameStatus, float currentSeconds);
    void DrawGameOver(sf::RenderWindow &window);

    void DrawGhostScore(sf::RenderWindow &window, int ghostPoints, const sf::Vector2f &ghostPosition);
    void QueueFruitPointsDisplay(float startTime, int fruitPoints, const sf::Vector2f &position);
    void ForceClearFruitPoints();

private:
    sf::Font &font;
    sf::Text menuTitle;
    sf::Text readyText;
    sf::Text scoreLabelText;
    sf::Text scoreText;

    float fruitPointsStartTime;
    sf::Text fruitPointsText;

private:
    sf::Text GetMenuTitle();
    sf::Text GetReadyText();
    sf::Text GetScoreLabelText();
    sf::Text GetScoreText();
    sf::Text GetGameOverText();
    sf::Text GetGhostPointsText(int ghostPoints, const sf::Vector2f &position);
    sf::Text GetFruitPointsText(int fruitPoints, const sf::Vector2f &position);
    void DisplayPauseSymbol(sf::RenderWindow &window);
    void SetScale(sf::Text &text);
    std::string Pad(std::string str, int width, char c);

    void ClearQueuedFruitPoints();
};

#endif // TEXTMANAGER_H

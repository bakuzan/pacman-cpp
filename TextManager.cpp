#include "pch.h"

#include <iomanip> // For std::setw and std::setfill
#include <sstream> // For std::stringstream

#include "include/Constants.h"
#include "include/TextManager.h"

TextManager::TextManager(sf::Font &font) : font(font)
{
    menuTitle = GetMenuTitle();
    readyText = GetReadyText();
    scoreLabelText = GetScoreLabelText();
    scoreText = GetScoreText();

    ClearQueuedFruitPoints();
}

TextManager::~TextManager()
{
    // Destructor
}

void TextManager::DrawMenuTitle(sf::RenderWindow &window)
{
    window.draw(menuTitle);
}

void TextManager::UpdateScoreText(int score)
{
    std::string paddedScore = Pad(std::to_string(score), 2, '0');
    std::string formattedScore = Pad(paddedScore, 7, ' ');
    scoreText.setString(formattedScore);
}

void TextManager::DrawPreGame(sf::RenderWindow &window)
{
    window.draw(readyText);
}

void TextManager::Draw(sf::RenderWindow &window, const GameStatus &gameStatus, float currentSeconds)
{
    window.draw(scoreLabelText);
    window.draw(scoreText);

    if (fruitPointsStartTime != 0.0f)
    {
        if ((currentSeconds - fruitPointsStartTime) > 1.5f)
        {
            ClearQueuedFruitPoints();
        }
        else
        {
            window.draw(fruitPointsText);
        }
    }

    if (gameStatus == GameStatus::PAUSED)
    {
        DisplayPauseSymbol(window);
    }
}

void TextManager::DrawGameOver(sf::RenderWindow &window)
{
    sf::Text gameOverText = GetGameOverText();
    window.draw(gameOverText);
}

void TextManager::DrawGhostScore(sf::RenderWindow &window, int ghostPoints, const sf::Vector2f &ghostPosition)
{
    sf::Text pointsText = GetGhostPointsText(ghostPoints, ghostPosition);
    window.draw(pointsText);
}

void TextManager::QueueFruitPointsDisplay(float startTime, int fruitPoints, const sf::Vector2f &position)
{
    fruitPointsStartTime = startTime;
    fruitPointsText = GetFruitPointsText(fruitPoints, position);
}

void TextManager::ForceClearFruitPoints()
{
    ClearQueuedFruitPoints();
}

// Private
sf::Text TextManager::GetMenuTitle()
{
    sf::Text menuTitleText;
    menuTitleText.setFont(font);
    menuTitleText.setString("PACMAN");
    menuTitleText.setCharacterSize(40);
    menuTitleText.setScale(4.0f / Constants::GRID_HEIGHT, 4.0f / Constants::GRID_HEIGHT);
    menuTitleText.setFillColor(sf::Color::Yellow);
    menuTitleText.setPosition(Constants::GRID_WIDTH / 2.0f, 5.0f);

    sf::FloatRect bounds = menuTitleText.getLocalBounds();
    menuTitleText.setOrigin(bounds.width / 2.0f,
                            bounds.height / 2.0f);

    return menuTitleText;
}

sf::Text TextManager::GetReadyText()
{
    sf::Text readyText;
    readyText.setFont(font);
    readyText.setString("READY!");
    SetScale(readyText);
    readyText.setFillColor(sf::Color::Yellow);
    readyText.setPosition(11.5f, Constants::GRID_OFFSET_Y + 16.75f);
    return readyText;
}

sf::Text TextManager::GetScoreLabelText()
{
    sf::Text scoreLabelText;
    scoreLabelText.setFont(font);
    scoreLabelText.setString(Pad("1UP", 7, ' '));
    SetScale(scoreLabelText);
    scoreLabelText.setFillColor(sf::Color::White);
    scoreLabelText.setPosition(5.0f, 0.25f);
    return scoreLabelText;
}

sf::Text TextManager::GetScoreText()
{
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setString(Pad("00", 7, ' '));
    SetScale(scoreText);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(5.5f, 1.25f);
    return scoreText;
}

sf::Text TextManager::GetGameOverText()
{
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("GAME   OVER");
    SetScale(gameOverText);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(10.5f, Constants::GRID_OFFSET_Y + 16.75f);
    return gameOverText;
}

sf::Text TextManager::GetGhostPointsText(int ghostPoints, const sf::Vector2f &position)
{
    sf::Text pointsText;
    pointsText.setFont(font);
    pointsText.setString(std::to_string(ghostPoints));
    pointsText.setCharacterSize(18);
    pointsText.setScale(1.0f / Constants::GRID_HEIGHT, 1.0f / Constants::GRID_HEIGHT);
    pointsText.setFillColor(sf::Color::Cyan);
    pointsText.setPosition(position);
    return pointsText;
}

sf::Text TextManager::GetFruitPointsText(int fruitPoints, const sf::Vector2f &position)
{
    sf::Text pointsText;
    pointsText.setFont(font);
    pointsText.setString(std::to_string(fruitPoints));
    pointsText.setCharacterSize(18);
    pointsText.setScale(1.0f / Constants::GRID_HEIGHT, 1.0f / Constants::GRID_HEIGHT);
    pointsText.setFillColor(sf::Color::Magenta);
    pointsText.setPosition(position.x - 0.5f, position.y - 0.25f);
    return pointsText;
}

void TextManager::DisplayPauseSymbol(sf::RenderWindow &window)
{
    float w = 2.0f;
    float h = 1.5f;
    float w2 = w / 5.0f;

    float x = Constants::GRID_WIDTH - (w / 2.0f) - 1.5f;
    float y = (h / 2.0f) + 0.5f;

    sf::RectangleShape pauseSymbol;
    pauseSymbol.setSize(sf::Vector2f(w, h));
    pauseSymbol.setPosition(x, y);
    pauseSymbol.setOrigin(w / 2.0f, h / 2.0f);
    pauseSymbol.setFillColor(sf::Color::White);

    sf::RectangleShape gap;
    gap.setSize(sf::Vector2f(w2, h));
    gap.setPosition(x, y);
    gap.setOrigin(w2 / 2.0f, h / 2.0f);
    gap.setFillColor(sf::Color(31, 31, 31));

    window.draw(pauseSymbol);
    window.draw(gap);
}

void TextManager::SetScale(sf::Text &text)
{
    text.setCharacterSize(20);
    text.setScale(1.0f / Constants::GRID_HEIGHT, 1.0f / Constants::GRID_HEIGHT);
}

std::string TextManager::Pad(std::string str, int width, char c)
{
    std::stringstream ss;
    ss << std::setw(width) << std::setfill(c) << str;
    return ss.str();
}

void TextManager::ClearQueuedFruitPoints()
{
    fruitPointsStartTime = 0.0f;
    fruitPointsText = sf::Text();
}
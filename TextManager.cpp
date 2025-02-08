#include <iomanip> // For std::setw and std::setfill
#include <sstream> // For std::stringstream
#include <string>  // For std::to_string

#include "include/Constants.h"
#include "include/TextManager.h"

TextManager::TextManager(sf::Font &font) : font(font)
{
    readyText = GetReadyText();
    scoreLabelText = GetScoreLabelText();
    scoreText = GetScoreText();
}

TextManager::~TextManager()
{
    // Destructor
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

void TextManager::Draw(sf::RenderWindow &window)
{
    window.draw(scoreLabelText);
    window.draw(scoreText);
}

// Private
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
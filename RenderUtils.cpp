#include "pch.h"

#include "include/RenderUtils.h"
#include "include/GameState.h"
#include "include/Constants.h"

void RefreshView(sf::RenderWindow &window, sf::View &view)
{
    window.clear(sf::Color(31, 31, 31));
    window.setView(view);
}

void DrawMazeEnvironment(sf::RenderWindow &window, std::optional<sf::Color> colour)
{
    bool setWallColour = colour.has_value();
    for (auto &wall : GameState::walls)
    {
        if (setWallColour &&
            wall.type == CellType::WALL)
        {
            wall.shape.setOutlineColor(colour.value());
        }
        window.draw(wall.shape);
    }

    for (auto &pu : GameState::pickUps)
    {
        if (pu.show)
        {
            window.draw(pu.shape);
        }
    }

    for (auto &frt : GameState::fruits)
    {
        frt.Draw(window);
    }
}

void DrawFooterGameStatuses(sf::RenderWindow &window, int lives)
{
    float xOffset = 6.0f;
    for (int i = 0; i < lives - 1; ++i)
    {
        sf::Sprite life = GameState::pacmanLives[i];
        life.setPosition(xOffset + float(i), Constants::GRID_HEIGHT - 0.75f);
        window.draw(life);
    }

    for (auto &fruit : GameState::fruits)
    {
        fruit.DrawStatusIcon(window);
    }
}

void DrawGhosts(sf::RenderWindow &window, std::optional<GhostPersonality> skipGhostPersonality)
{
    for (auto &gst : GameState::ghosts)
    {
        if (!skipGhostPersonality.has_value() ||
            gst.GetPersonality() != skipGhostPersonality.value())
        {
            gst.Draw(window);
        }
    }
}

void FlashWalls(sf::RenderWindow &window, sf::View &view, TextManager &textManager)
{
    float duration = 2.5f;
    float interval = 0.25f;
    float startTime = GameState::gameClock.getElapsedTime().asSeconds();

    while (GameState::gameClock.getElapsedTime().asSeconds() - startTime < duration)
    {
        float elapsed = GameState::gameClock.getElapsedTime().asSeconds() - startTime;
        sf::Color colour = (static_cast<int>(elapsed / interval) % 2 == 0)
                               ? sf::Color::White
                               : Constants::WALL_COLOUR;

        RefreshView(window, view);
        DrawMazeEnvironment(window, colour);
        DrawFooterGameStatuses(window, GameState::lives);
        textManager.Draw(window, GameState::gameStatus, GameState::gameClock.getElapsedTime().asSeconds());
        window.display();

        sf::sleep(sf::milliseconds(50));
    }
}

void HideFruits(TextManager &textManager)
{
    textManager.ForceClearFruitPoints();

    for (auto &frt : GameState::fruits)
    {
        frt.Hide();
    }
}

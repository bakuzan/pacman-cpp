#include "pch.h"

#include "include/GameInit.h"
#include "include/GameState.h"
#include "include/SFMLUtils.h"
#include "include/Constants.h"

void PopulateFruit(sf::Texture &texture)
{
    GameState::fruits.push_back(Fruit(texture, 1.0f, 30.0f));
    GameState::fruits.push_back(Fruit(texture, 1.0f, 70.0f));
}

void PopulatePacmanLives(sf::Texture &texture)
{
    for (int i = 0; i < 3; ++i)
    {
        sf::Sprite pacmanLife;
        pacmanLife.setTexture(texture);
        pacmanLife.setTextureRect(sf::IntRect(0, 0, 32, 32));
        SFMLUtils::CenterOriginAndScale(pacmanLife, 1.0f);
        GameState::pacmanLives.push_back(pacmanLife);
    }
}

void InitialiseGame(Player &player, GhostModeController *ghostModeController, bool isNewGame)
{
    GameState::gameStatus = GameStatus::PRE_GAME;
    GameState::gameClock.restart();
    GameState::deltaClock.restart();
    GameState::deltaTime = 0.0f;

    if (isNewGame)
    {
        GameState::score = 0;
        GameState::lives = 3;
        GameState::level = 1;
    }
    else
    {
        GameState::level++;
    }

    player.Reset();
    ghostModeController->Reset();

    for (auto &gst : GameState::ghosts)
    {
        gst.Reset();
    }

    for (auto &pus : GameState::pickUps)
    {
        pus.show = true;
    }

    for (auto &frt : GameState::fruits)
    {
        frt.Reset(GameState::level);
    }

    for (auto &wall : GameState::walls)
    {
        if (wall.type == CellType::WALL)
        {
            wall.shape.setOutlineColor(Constants::WALL_COLOUR);
        }
    }
}

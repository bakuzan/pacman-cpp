#include "pch.h"

#include "GameState.h"

namespace GameState
{
    GameStatus gameStatus = GameStatus::MENU;
    float displayDuration = 3.0f;
    sf::Clock gameClock;
    float deltaTime = 0.0f;
    sf::Clock deltaClock;
    int score = 0;
    int lives = 3;
    int level = 1;
    std::vector<Wall> walls;
    std::vector<Ghost> ghosts;
    std::vector<PickUp> pickUps;
    std::vector<Fruit> fruits;
    std::vector<sf::Sprite> pacmanLives;
}

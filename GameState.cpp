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
    std::vector<sf::Vector2f> intersections;
    std::vector<Wall> walls;
    std::vector<Ghost> ghosts;
    std::vector<PickUp> pickUps;
    std::vector<Fruit> fruits;
    std::vector<sf::Sprite> pacmanLives;
    // Sounds
    std::vector<sf::SoundBuffer> soundBuffers;

    sf::Sound menuMusic;
    sf::Sound introMusic;
    sf::Sound backgroundMusic;
    sf::Sound frightenedMusic;

    sf::Sound pacmanMovingSound;
    sf::Sound pacmanEatFruitSound;
    sf::Sound pacmanEatGhostSound;
    sf::Sound pacmanDyingSound;
    sf::Sound ghostRetreatSound;
}

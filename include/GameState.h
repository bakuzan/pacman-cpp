#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <vector>

#include "GameStatus.h"
#include "Wall.h"
#include "Ghost.h"
#include "PickUp.h"
#include "Fruit.h"

namespace GameState
{
    extern GameStatus gameStatus;
    extern float displayDuration;
    extern sf::Clock gameClock;
    extern float deltaTime;
    extern sf::Clock deltaClock;
    extern int score;
    extern int lives;
    extern int level;
    extern std::vector<sf::Vector2f> intersections;
    extern std::vector<Wall> walls;
    extern std::vector<Ghost> ghosts;
    extern std::vector<PickUp> pickUps;
    extern std::vector<Fruit> fruits;
    extern std::vector<sf::Sprite> pacmanLives;
    // Sounds
    extern std::vector<sf::SoundBuffer> soundBuffers;

    extern sf::Sound menuMusic;
    extern sf::Sound introMusic;
    extern sf::Sound backgroundMusic;
    extern sf::Sound frightenedMusic;

    extern sf::Sound pacmanMovingSound;
    extern sf::Sound pacmanEatFruitSound;
    extern sf::Sound pacmanEatGhostSound;
    extern sf::Sound pacmanDyingSound;
    extern sf::Sound ghostRetreatSound;
}

#endif // GAMESTATE_H

#include "pch.h"
#include <SFML/Audio.hpp>

#include <algorithm>
#include <fstream>
#include <sstream>

#include "include/FileUtils.h"
#include "include/Constants.h"
#include "include/CellType.h"
#include "include/GameState.h"
#include "FileUtils.h"

// Private Helpers

void LoadSoundBuffer(sf::SoundBuffer &buffer, const std::string &fileName)
{
    if (!buffer.loadFromFile(fileName))
    {
        WriteToLogFile("Error loading file " + fileName);
        exit(EXIT_FAILURE);
    }
}

// Private Helpers END

void WriteToLogFile(const std::string &message)
{
    std::ofstream logFile("./logs/log.txt", std::ios_base::app);
    logFile << message << std::endl;
    logFile.close();
}

void ReadAndProcessMap(sf::Texture &sharedTexture, Player &player)
{
    std::ifstream file("./resources/map.txt");
    if (!file.is_open())
    {
        WriteToLogFile("Error opening file ./resources/map.txt");
        exit(EXIT_FAILURE);
    }

    std::vector<sf::Vector2f> walkableSpots;
    int ghostColumnIndex = 0;
    std::string line;
    int lineIndex = 0;
    while (getline(file, line))
    {
        std::stringstream ss(line);
        std::string number;
        int numberIndex = 0;

        while (getline(ss, number, ','))
        {
            int cellContents = std::stoi(number);
            float x = float(numberIndex);
            float y = float(lineIndex) + Constants::GRID_OFFSET_Y;

            switch (cellContents)
            {
            case CellType::WALL:
            {
                sf::RectangleShape wall;
                wall.setSize(sf::Vector2f(0.6f, 0.6f));
                wall.setOrigin(wall.getSize().x / 2.0f, wall.getSize().y / 2.0f);
                wall.setFillColor(sf::Color::Transparent);
                wall.setOutlineColor(Constants::WALL_COLOUR);
                wall.setOutlineThickness(0.20f);
                wall.setPosition(x, y);
                GameState::walls.push_back({CellType::WALL, wall});
                break;
            }
            case CellType::GHOST_DOOR:
            {
                sf::RectangleShape ghostDoor;
                ghostDoor.setSize(sf::Vector2f(1.0f, 1.0f));
                ghostDoor.setOrigin(ghostDoor.getSize().x / 2.0f, ghostDoor.getSize().y / 2.0f);
                ghostDoor.setFillColor(sf::Color(100, 41, 71));
                ghostDoor.setPosition(x, y);
                GameState::walls.push_back({CellType::GHOST_DOOR, ghostDoor});
                break;
            }
            case CellType::PELLET:
            {
                float size = 0.1;
                sf::CircleShape cs(size);
                cs.setOrigin(size, size);
                cs.setFillColor(sf::Color(255, 229, 180));
                cs.setPosition(x, y);
                GameState::pickUps.push_back({CellType::PELLET, cs, true});
                break;
            }
            case CellType::POWER_UP:
            {
                float size = 0.4;
                sf::CircleShape cs(size);
                cs.setOrigin(size, size);
                cs.setFillColor(sf::Color(255, 229, 180));
                cs.setPosition(x, y);
                GameState::pickUps.push_back({CellType::POWER_UP, cs, true});
                break;
            }
            case CellType::GHOST_START_POSITION:
            {
                Ghost gst(sharedTexture, Constants::SPRITE_SIZE, ghostColumnIndex++);
                gst.SetSpawnPosition(x, y);
                GameState::ghosts.push_back(gst);
                break;
            }
            case CellType::PACMAN_START_POSITION:
            {
                player.SetSpawnPosition(x, y);
                break;
            }
            case CellType::NOTHING:
            default:
                break;
            }

            // Gather list of walkables
            switch (cellContents)
            {
            case CellType::WALL:
            case CellType::GHOST_DOOR:
                break;
            default:
            {
                walkableSpots.push_back({x, y});
                break;
            }
            }

            numberIndex++;
        }

        lineIndex++;
    }
    file.close();

    // Build list of intersections
    for (const auto &spot : walkableSpots)
    {
        int pathCount = 0;

        // Check all four directions
        if (std::find(walkableSpots.begin(), walkableSpots.end(), sf::Vector2f{spot.x - 1.0f, spot.y}) != walkableSpots.end())
            pathCount++;
        if (std::find(walkableSpots.begin(), walkableSpots.end(), sf::Vector2f{spot.x + 1.0f, spot.y}) != walkableSpots.end())
            pathCount++;
        if (std::find(walkableSpots.begin(), walkableSpots.end(), sf::Vector2f{spot.x, spot.y - 1.0f}) != walkableSpots.end())
            pathCount++;
        if (std::find(walkableSpots.begin(), walkableSpots.end(), sf::Vector2f{spot.x, spot.y + 1.0f}) != walkableSpots.end())
            pathCount++;

        // Intersection if more than 2 paths available
        if (pathCount > 2)
        {
            GameState::intersections.push_back(spot);
        }
    }
}

void LoadSoundFiles()
{
    float lowerSoundLevelForGratingSounds = 80.0f;

    GameState::soundBuffers.resize(9);

    LoadSoundBuffer(GameState::soundBuffers[0], "./resources/paccutscene.wav");
    GameState::menuMusic.setBuffer(GameState::soundBuffers[0]);
    GameState::menuMusic.setVolume(lowerSoundLevelForGratingSounds);

    LoadSoundBuffer(GameState::soundBuffers[1], "./resources/pacstart.wav");
    GameState::introMusic.setBuffer(GameState::soundBuffers[1]);
    GameState::introMusic.setVolume(lowerSoundLevelForGratingSounds);

    LoadSoundBuffer(GameState::soundBuffers[2], "./resources/pacghost1.wav");
    GameState::backgroundMusic.setBuffer(GameState::soundBuffers[2]);
    GameState::backgroundMusic.setLoop(true);
    GameState::backgroundMusic.setVolume(lowerSoundLevelForGratingSounds);

    LoadSoundBuffer(GameState::soundBuffers[3], "./resources/pacghostblue.wav");
    GameState::frightenedMusic.setBuffer(GameState::soundBuffers[3]);
    GameState::frightenedMusic.setLoop(true);
    GameState::frightenedMusic.setVolume(lowerSoundLevelForGratingSounds);

    // Reactive sounds
    LoadSoundBuffer(GameState::soundBuffers[4], "./resources/pacdot1.wav");
    GameState::pacmanMovingSound.setBuffer(GameState::soundBuffers[4]);
    GameState::pacmanMovingSound.setLoop(true);
    GameState::pacmanMovingSound.setVolume(lowerSoundLevelForGratingSounds);

    LoadSoundBuffer(GameState::soundBuffers[5], "./resources/pacfruit.wav");
    GameState::pacmanEatFruitSound.setBuffer(GameState::soundBuffers[5]);

    LoadSoundBuffer(GameState::soundBuffers[6], "./resources/pacghosteat.wav");
    GameState::pacmanEatGhostSound.setBuffer(GameState::soundBuffers[6]);

    LoadSoundBuffer(GameState::soundBuffers[7], "./resources/pacdie1.wav");
    GameState::pacmanDyingSound.setBuffer(GameState::soundBuffers[7]);

    LoadSoundBuffer(GameState::soundBuffers[8], "./resources/pacghostretreat.wav");
    GameState::ghostRetreatSound.setBuffer(GameState::soundBuffers[8]);
    GameState::ghostRetreatSound.setLoop(true);
    GameState::ghostRetreatSound.setVolume(lowerSoundLevelForGratingSounds);
}

void StopAllSounds()
{
    GameState::menuMusic.stop();
    GameState::introMusic.stop();
    GameState::backgroundMusic.stop();
    GameState::frightenedMusic.stop();

    // Reactive sounds
    GameState::pacmanMovingSound.stop();
    GameState::pacmanEatFruitSound.stop();
    GameState::pacmanEatGhostSound.stop();
    GameState::pacmanDyingSound.stop();
    GameState::ghostRetreatSound.stop();
}

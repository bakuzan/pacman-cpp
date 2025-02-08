#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>

#include "include/Constants.h"
#include "include/CellType.h"
#include "include/Ghost.h"
#include "include/Player.h"
#include "include/PickUp.h"
#include "include/TextManager.h"
#include "include/Wall.h"

std::vector<Wall> walls;
std::vector<Ghost> ghosts;
std::vector<PickUp> pickUps;

void writeToLogFile(const std::string &message)
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
        writeToLogFile("Error opening file ./resources/map.txt");
        exit(EXIT_FAILURE);
    }

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
                wall.setOutlineColor(sf::Color(33, 33, 222));
                wall.setOutlineThickness(0.20f);
                wall.setPosition(x, y);
                walls.push_back({CellType::WALL, wall});
                break;
            }
            case CellType::GHOST_DOOR:
            {
                sf::RectangleShape ghostDoor;
                ghostDoor.setSize(sf::Vector2f(1.0f, 1.0f));
                ghostDoor.setOrigin(ghostDoor.getSize().x / 2.0f, ghostDoor.getSize().y / 2.0f);
                ghostDoor.setFillColor(sf::Color(100, 41, 71));
                ghostDoor.setPosition(x, y);
                walls.push_back({CellType::GHOST_DOOR, ghostDoor});
                break;
            }
            case CellType::PELLET:
            {
                float size = 0.1;
                sf::CircleShape cs(size);
                cs.setOrigin(size, size);
                cs.setFillColor(sf::Color(255, 229, 180));
                cs.setPosition(x, y);
                pickUps.push_back({CellType::PELLET, cs, true});
                break;
            }
            case CellType::POWER_UP:
            {
                float size = 0.4;
                sf::CircleShape cs(size);
                cs.setOrigin(size, size);
                cs.setFillColor(sf::Color(255, 229, 180));
                cs.setPosition(x, y);
                pickUps.push_back({CellType::POWER_UP, cs, true});
                break;
            }
            case CellType::GHOST_START_POSITION:
            {
                Ghost gst(sharedTexture, Constants::SPRITE_SIZE, ghostColumnIndex++);
                gst.SetSpawnPosition(x, y);
                ghosts.push_back(gst);
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

            numberIndex++;
        }

        lineIndex++;
    }

    // Close the file when done
    file.close();
}

int main()
{
    std::srand(std::time(0));
    sf::RenderWindow window(sf::VideoMode(Constants::WINDOW_SIZE, Constants::WINDOW_SIZE), "Pacman C++", sf::Style::Close | sf::Style::Resize);
    sf::View view(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(Constants::GRID_WIDTH, Constants::GRID_HEIGHT));
    view.setCenter(Constants::GRID_WIDTH / 2.0f, Constants::GRID_HEIGHT / 2.0f);

    // Load spritesheet
    sf::Image spritesheet;
    if (!spritesheet.loadFromFile("./resources/spritesheet.png"))
    {
        writeToLogFile("Error opening file ./resources/spritesheet.png");
        exit(EXIT_FAILURE);
    }

    // Set colour that will be treated as transparent!
    sf::Color transparentColor = spritesheet.getPixel(0, 0);
    spritesheet.createMaskFromColor(transparentColor);

    sf::Texture sharedTexture;
    sharedTexture.loadFromImage(spritesheet);

    // Load font for display text
    sf::Font font;
    if (!font.loadFromFile("./resources/PressStart2P-Regular.ttf"))
    {
        writeToLogFile("Error loading font");
        exit(EXIT_FAILURE);
    }

    TextManager textManager(font);

    // Create player
    Player player = Player(sharedTexture, Constants::SPRITE_SIZE);

    // Process map
    ReadAndProcessMap(sharedTexture, player);

    // Get min/max x coordinate for the "teleport" tunnel
    auto [minWall, maxWall] = std::minmax_element(walls.begin(), walls.end(),
                                                  [](const Wall &a, const Wall &b)
                                                  {
                                                      return a.shape.getPosition().x < b.shape.getPosition().x;
                                                  });

    float minX = minWall->shape.getPosition().x;
    float maxX = maxWall->shape.getPosition().x;

    // Game state
    bool isPreGame = true;
    float displayDuration = 3.0f;
    sf::Clock readyClock;

    float deltaTime = 0.0f;
    sf::Clock clock;

    int score = 0;
    GhostModeController *ghostModeController = GhostModeController::GetInstance();

    while (window.isOpen())
    {
        // Timing
        deltaTime = clock.restart().asSeconds();
        float fpsLimit = 1.0f / 20.0f; // Lock fps
        if (deltaTime > fpsLimit)
        {
            deltaTime = fpsLimit;
        }

        // Input
        sf::Event evnt;
        while (window.pollEvent(evnt))
        {
            switch (evnt.type)
            {
            case sf::Event::Closed:
            {
                window.close();
                break;
            }
            case sf::Event::Resized:
            {
                // Maintain the height of the view to match the window height
                float aspectRatio = float(window.getSize().x) / float(window.getSize().y);
                view.setSize(Constants::GRID_HEIGHT * aspectRatio, Constants::GRID_HEIGHT);
                break;
            }
            }
        }

        // Player Input
        Direction newDirection = Direction::NONE;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            newDirection = Direction::LEFT;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            newDirection = Direction::RIGHT;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            newDirection = Direction::UP;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            newDirection = Direction::DOWN;
        }

        // Logic
        if (!isPreGame)
        {
            // Tick ghost modes
            ghostModeController->Update(deltaTime, ghosts);

            // Advance player
            player.Update(newDirection, deltaTime, walls, minX, maxX);

            // Did we eat pellets?
            sf::Vector2f playerPos = player.GetPosition();
            int playerX = static_cast<int>(std::round(playerPos.x));
            int playerY = static_cast<int>(std::round(playerPos.y));
            for (auto &pu : pickUps)
            {
                auto puPos = pu.shape.getPosition();
                if (playerX == puPos.x &&
                    playerY == puPos.y &&
                    pu.show)
                {
                    pu.show = false;

                    if (pu.type == CellType::POWER_UP)
                    {
                        ghostModeController->StartFrightened();
                        score += 50;
                    }
                    else
                    {
                        score += 10;
                    }
                }
            }

            // Advance ghosts
            sf::FloatRect playerRect(playerPos, sf::Vector2f(Constants::SPRITE_SIZE, Constants::SPRITE_SIZE));
            for (auto &gst : ghosts)
            {
                gst.Update(deltaTime, walls, ghosts, player, minX, maxX);

                // Did the ghosts get pacman? Or did pacman get frightened ghosts?
                sf::Vector2f ghostPos = gst.GetPosition();
                sf::FloatRect ghostRect(ghostPos, sf::Vector2f(Constants::SPRITE_SIZE, Constants::SPRITE_SIZE));

                if (ghostRect.intersects(playerRect))
                {
                    GhostPersonality ghostPersonality = gst.GetPersonality();
                    GhostMode ghostMode = ghostModeController->GetMode(ghostPersonality);
                    if (ghostMode == GhostMode::FRIGHTENED)
                    {
                        ghostModeController->Eaten(ghostPersonality);
                        score += 200; // TODO 200,400,800,1600
                    }
                    else
                    {
                        // TODO
                        // Implement the ghost getting pacman!
                    }
                }
            }
        }

        textManager.UpdateScoreText(score);

        // Draw+Display
        window.clear(sf::Color(31, 31, 31));
        window.setView(view);

        textManager.Draw(window);

        if (isPreGame)
        {
            textManager.DrawPreGame(window);

            if (readyClock.getElapsedTime().asSeconds() >= displayDuration)
            {
                isPreGame = false;
                player.SetDirection(Direction::LEFT);
            }
        }

        for (auto &wall : walls)
        {
            window.draw(wall.shape);
        }

        for (auto &pu : pickUps)
        {
            if (pu.show)
            {
                window.draw(pu.shape);
            }
        }

        player.Draw(window);

        for (auto &gst : ghosts)
        {
            gst.Draw(window);
        }

        // Display new window
        window.display();
    }

    return 0;
}
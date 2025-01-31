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
                cs.setFillColor(sf::Color::White);
                cs.setPosition(x, y);
                pickUps.push_back({CellType::PELLET, cs});
                break;
            }
            case CellType::POWER_UP:
            {
                float size = 0.4;
                sf::CircleShape cs(size);
                cs.setOrigin(size, size);
                cs.setFillColor(sf::Color::White);
                cs.setPosition(x, y);
                pickUps.push_back({CellType::POWER_UP, cs});
                break;
            }
            case CellType::GHOST_START_POSITION:
            {
                Ghost gst(sharedTexture, Constants::SPRITE_SIZE, ghostColumnIndex++);
                gst.SetPosition(x, y);
                ghosts.push_back(gst);
                break;
            }
            case CellType::PACMAN_START_POSITION:
            {
                player.SetPosition(x, y);
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

    sf::Text readyText;
    readyText.setFont(font);
    readyText.setString("READY!");
    readyText.setCharacterSize(20);
    readyText.setScale(1.0f / 30.0f, 1.0f / 30.0f);
    readyText.setFillColor(sf::Color::Yellow);
    readyText.setPosition(11.5f, 17.75f);

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
            player.Update(newDirection, deltaTime, walls, minX, maxX);

            ghostModeController->Update(deltaTime, ghosts);

            for (auto &gst : ghosts)
            {
                gst.Update(deltaTime, walls, ghosts, player, minX, maxX);
            }

            // Did the ghosts get pacman?
            // TODO

            // Eat pellets?
            sf::Vector2f playerPos = player.GetPosition();
            int playerX = static_cast<int>(std::round(playerPos.x));
            int playerY = static_cast<int>(std::round(playerPos.y));
            pickUps.erase(std::remove_if(pickUps.begin(), pickUps.end(),
                                         [&playerX, &playerY](const PickUp &pu)
                                         {
                                             auto puPos = pu.shape.getPosition();
                                             if (playerX == puPos.x &&
                                                 playerY == puPos.y)
                                             {
                                                 return true;
                                             }
                                             return false;
                                         }),
                          pickUps.end());
        }

        // Draw+Display
        window.clear(sf::Color(31, 31, 31));
        window.setView(view);

        if (isPreGame)
        {
            window.draw(readyText);

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
            window.draw(pu.shape);
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
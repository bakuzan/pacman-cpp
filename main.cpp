#include <SFML/Graphics.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <sstream>
#include <thread>
#include <vector>

#include "include/Button.h"
#include "include/Constants.h"
#include "include/CellType.h"
#include "include/EnumUtils.h"
#include "include/Ghost.h"
#include "include/Player.h"
#include "include/PickUp.h"
#include "include/SFMLUtils.h"
#include "include/TextManager.h"
#include "include/Wall.h"

// Game State
bool isInGame = false;
bool isPreGame = true;
bool isGameOver = false;

float displayDuration = 3.0f;
sf::Clock gameClock;

float deltaTime = 0.0f;
sf::Clock deltaClock;

int score = 0;
int lives = 3;

std::vector<Wall> walls;
std::vector<Ghost> ghosts;
std::vector<PickUp> pickUps;
std::vector<sf::Sprite> pacmanLives;

// Game State END

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

void PopulatePacmanLives(sf::Texture &texture)
{
    for (int i = 0; i < 3; ++i)
    {
        sf::Sprite pacmanLife;
        pacmanLife.setTexture(texture);
        pacmanLife.setTextureRect(sf::IntRect(0, 0, 32, 32));
        SFMLUtils::CenterOriginAndScale(pacmanLife, 1.0f);
        pacmanLives.push_back(pacmanLife);
    }
}

void RefreshView(sf::RenderWindow &window, sf::View &view)
{
    window.clear(sf::Color(31, 31, 31));
    window.setView(view);
}

void DrawMazeEnvironment(sf::RenderWindow &window)
{
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
}

void DrawPacmanLives(sf::RenderWindow &window, int lives)
{
    float xOffset = 6.0f;
    for (int i = 0; i < lives - 1; ++i)
    {
        sf::Sprite life = pacmanLives[i];
        life.setPosition(xOffset + float(i), Constants::GRID_HEIGHT - 0.75f);
        window.draw(life);
    }
}

void DrawGhosts(sf::RenderWindow &window, std::optional<GhostPersonality> skipGhostPersonality = std::nullopt)
{
    for (auto &gst : ghosts)
    {
        if (!skipGhostPersonality.has_value() ||
            gst.GetPersonality() != skipGhostPersonality.value())
        {
            gst.Draw(window);
        }
    }
}

void InitialiseGame(Player &player, GhostModeController *ghostModeController)
{
    isInGame = true;
    isPreGame = true;
    isGameOver = false;

    gameClock.restart();
    deltaClock.restart();
    deltaTime = 0.0f;

    score = 0;
    lives = 3;

    player.Reset();

    for (auto &gst : ghosts)
    {
        gst.Reset();
        ghostModeController->ResetToHouse(gst.GetPersonality(), true);
    }

    for (auto &pus : pickUps)
    {
        pus.show = true;
    }
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
    PopulatePacmanLives(sharedTexture);

    // Load font for display text
    sf::Font font;
    if (!font.loadFromFile("./resources/PressStart2P-Regular.ttf"))
    {
        writeToLogFile("Error loading font");
        exit(EXIT_FAILURE);
    }

    // Create managers, controllers, and player
    TextManager textManager(font);
    Player player = Player(sharedTexture, Constants::SPRITE_SIZE);
    GhostModeController *ghostModeController = GhostModeController::GetInstance();

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

    // Menu
    float btnWidth = 30.0f;
    float btnHeight = 5.0f;
    float btnX = Constants::GRID_WIDTH / 2.0f;

    Button newGameButton(font, btnX, Constants::GRID_HEIGHT / 2.0f - btnHeight / 2.0f, btnWidth, btnHeight, "New Game", [&player, &ghostModeController]()
                         { InitialiseGame(player, ghostModeController); });
    Button quitButton(font, btnX, Constants::GRID_HEIGHT / 2.0f + btnHeight / 2.0f + 5.0f, btnWidth, btnHeight, "Quit", [&window]()
                      { window.close(); });

    // Menu END

    while (window.isOpen())
    {
        // Timing
        deltaTime = deltaClock.restart().asSeconds();
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

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            newGameButton.HandleEvent(evnt, mousePos);
            quitButton.HandleEvent(evnt, mousePos);
        }

        if (isInGame)
        {
            if (!isGameOver)
            {
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

                        sf::FloatRect intersection;
                        if (ghostRect.intersects(playerRect, intersection))
                        {
                            float intersectionArea = intersection.width * intersection.height;
                            if (intersectionArea < 0.2f)
                            {
                                continue; // Ignoring overlaps that arent visible
                            }

                            GhostPersonality ghostPersonality = gst.GetPersonality();
                            GhostMode ghostMode = ghostModeController->GetMode(ghostPersonality);

                            if (ghostMode == GhostMode::FRIGHTENED)
                            {
                                ghostModeController->Eaten(ghostPersonality);
                                int frightenedGhosts = ghostModeController->GetFrightenedCount();
                                int gstPoints = std::pow(2, (4 - frightenedGhosts)) * 100;
                                score += gstPoints;
                                // Pause and show points earn for ghost!
                                RefreshView(window, view);
                                DrawMazeEnvironment(window);
                                DrawPacmanLives(window, lives);
                                textManager.Draw(window);
                                DrawGhosts(window, ghostPersonality);
                                textManager.DrawGhostScore(window, gstPoints, ghostPos);
                                window.display();
                                std::this_thread::sleep_for(std::chrono::milliseconds(1500));
                            }
                            else if (ghostMode != GhostMode::SPAWN)
                            {
                                RefreshView(window, view);
                                DrawMazeEnvironment(window);
                                DrawPacmanLives(window, lives);
                                textManager.Draw(window);
                                player.Draw(window);
                                DrawGhosts(window);
                                window.display();
                                std::this_thread::sleep_for(std::chrono::milliseconds(250));

                                bool isDying = true;
                                while (isDying)
                                {
                                    isDying = player.Dying();

                                    RefreshView(window, view);
                                    DrawMazeEnvironment(window);
                                    DrawPacmanLives(window, lives);
                                    textManager.Draw(window);
                                    player.Draw(window);
                                    window.display();

                                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                }

                                lives--;
                                if (lives == 0)
                                {
                                    isGameOver = true;
                                    gameClock.restart();
                                }
                                else
                                {
                                    player.Reset();

                                    for (auto &gst : ghosts)
                                    {
                                        gst.Reset();
                                    }

                                    isPreGame = true;
                                    gameClock.restart();
                                }
                            }
                        }
                    }
                }

                textManager.UpdateScoreText(score);

                // if logic block caused game over, then no need to render for this loop
                if (isGameOver)
                {
                    continue;
                }

                // Draw+Display
                RefreshView(window, view);
                DrawMazeEnvironment(window);
                DrawPacmanLives(window, lives);
                textManager.Draw(window);
                player.Draw(window);
                DrawGhosts(window);

                if (isPreGame)
                {
                    textManager.DrawPreGame(window);

                    if (gameClock.getElapsedTime().asSeconds() >= displayDuration)
                    {
                        isPreGame = false;
                        player.SetDirection(Direction::LEFT);
                    }
                }
            }
            else
            {
                RefreshView(window, view);
                DrawMazeEnvironment(window);
                DrawPacmanLives(window, lives);
                textManager.Draw(window);
                textManager.DrawGameOver(window);

                if (gameClock.getElapsedTime().asSeconds() >= displayDuration)
                {
                    isInGame = false; // Will return user to "New Game" Menu
                }
            }

            // Display new window
            window.display();
        }
        else
        {
            RefreshView(window, view);
            textManager.DrawMenuTitle(window);
            newGameButton.Draw(window);
            quitButton.Draw(window);

            // Display new window
            window.display();
        }
    }

    return 0;
}

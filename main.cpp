#include "pch.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <sstream>
#include <thread>

#include "include/Button.h"
#include "include/Constants.h"
#include "include/CellType.h"
#include "include/EnumUtils.h"
#include "include/Fruit.h"
#include "include/GameStatus.h"
#include "include/Ghost.h"
#include "include/Player.h"
#include "include/PickUp.h"
#include "include/SFMLUtils.h"
#include "include/TextManager.h"
#include "include/Wall.h"

// Game State
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
                wall.setOutlineColor(Constants::WALL_COLOUR);
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

void PopulateFruit(sf::Texture &texture)
{
    fruits.push_back(Fruit(texture, Constants::SPRITE_SIZE, 30.0f));
    fruits.push_back(Fruit(texture, Constants::SPRITE_SIZE, 70.0f));
}

void PopulatePacmanLives(sf::Texture &texture)
{
    for (int i = 0; i < 3; ++i)
    {
        sf::Sprite pacmanLife;
        pacmanLife.setTexture(texture);
        pacmanLife.setTextureRect(sf::IntRect(0, 0, 32, 32));
        SFMLUtils::CenterOriginAndScale(pacmanLife, Constants::SPRITE_SIZE);
        pacmanLives.push_back(pacmanLife);
    }
}

void RefreshView(sf::RenderWindow &window, sf::View &view)
{
    window.clear(sf::Color(31, 31, 31));
    window.setView(view);
}

void DrawMazeEnvironment(sf::RenderWindow &window, std::optional<sf::Color> colour = std::nullopt)
{
    bool setWallColour = colour.has_value();
    for (auto &wall : walls)
    {
        if (setWallColour &&
            wall.type == CellType::WALL)
        {
            wall.shape.setOutlineColor(colour.value());
        }

        window.draw(wall.shape);
    }

    for (auto &pu : pickUps)
    {
        if (pu.show)
        {
            window.draw(pu.shape);
        }
    }

    for (auto &frt : fruits)
    {
        frt.Draw(window);
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

void FlashWalls(sf::RenderWindow &window, sf::View &view, TextManager &textManager)
{
    float duration = 2.5f;
    float interval = 0.25f;
    float startTime = gameClock.getElapsedTime().asSeconds();

    while (gameClock.getElapsedTime().asSeconds() - startTime < duration)
    {
        float elapsed = gameClock.getElapsedTime().asSeconds() - startTime;
        sf::Color colour = (static_cast<int>(elapsed / interval) % 2 == 0)
                               ? sf::Color::White
                               : Constants::WALL_COLOUR;

        RefreshView(window, view);
        DrawMazeEnvironment(window, colour);
        DrawPacmanLives(window, lives);
        textManager.Draw(window, gameStatus, gameClock.getElapsedTime().asSeconds());
        window.display();

        sf::sleep(sf::milliseconds(50));
    }
}

void HideFruits(TextManager &textManager)
{
    textManager.ForceClearFruitPoints();

    for (auto &frt : fruits)
    {
        frt.Hide();
    }
}

void InitialiseGame(Player &player, GhostModeController *ghostModeController, bool isNewGame = true)
{
    gameStatus = GameStatus::PRE_GAME;

    gameClock.restart();
    deltaClock.restart();
    deltaTime = 0.0f;

    if (isNewGame)
    {
        score = 0;
        lives = 3;
        level = 1;
    }
    else
    {
        level++;
    }

    player.Reset();
    ghostModeController->Reset();

    for (auto &gst : ghosts)
    {
        gst.Reset();
    }

    for (auto &pus : pickUps)
    {
        pus.show = true;
    }

    for (auto &frt : fruits)
    {
        frt.Reset(level);
    }

    for (auto &wall : walls)
    {
        if (wall.type == CellType::WALL)
        {
            wall.shape.setOutlineColor(Constants::WALL_COLOUR);
        }
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
    PopulateFruit(sharedTexture);
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
            case sf::Event::KeyPressed:
            {
                if (evnt.key.code == sf::Keyboard::P)
                {
                    switch (gameStatus)
                    {
                    case GameStatus::IN_GAME:
                        gameStatus = GameStatus::PAUSED;
                        break;
                    case GameStatus::PAUSED:
                        gameStatus = GameStatus::IN_GAME;
                        break;
                    default:
                        break;
                    }
                }
                break;
            }
            }

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosView = window.mapPixelToCoords(mousePos);

            newGameButton.HandleEvent(evnt, mousePosView);
            quitButton.HandleEvent(evnt, mousePosView);
        }

        if (gameStatus == GameStatus::MENU)
        {
            RefreshView(window, view);
            textManager.DrawMenuTitle(window);
            newGameButton.Draw(window);
            quitButton.Draw(window);

            // Display new window
            window.display();
        }
        else
        {
            if (!(gameStatus == GameStatus::GAME_OVER))
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
                if (gameStatus == GameStatus::IN_GAME)
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

                    // Are all pellets eaten?
                    bool allPelletsEaten = std::all_of(pickUps.cbegin(), pickUps.cend(),
                                                       [](const auto &pu)
                                                       { return !pu.show; });
                    if (allPelletsEaten)
                    {
                        HideFruits(textManager);
                        FlashWalls(window, view, textManager);
                        InitialiseGame(player, ghostModeController, false);
                        continue;
                    }

                    // Fruit - Do we need to spawn a fruit? Hide one? Eaten it?
                    int totalPickups = pickUps.size();
                    int countNotShowing = std::count_if(pickUps.cbegin(), pickUps.cend(),
                                                        [](const PickUp &pickup)
                                                        { return !pickup.show; });

                    double percentageNotShowing = (static_cast<double>(countNotShowing) / totalPickups) * 100;
                    for (auto &frt : fruits)
                    {
                        float currentSeconds = gameClock.getElapsedTime().asSeconds();
                        if (frt.IsShown())
                        {
                            // Has Player eaten the fruit?
                            auto frtPos = frt.GetPosition();
                            if (playerX == frtPos.x &&
                                playerY == frtPos.y)
                            {
                                int fruitPoints = frt.GetPoints();
                                score += fruitPoints;
                                textManager.QueueFruitPointsDisplay(currentSeconds, fruitPoints, frtPos);
                                frt.Hide();
                            }

                            if (currentSeconds - frt.GetDisplayTime() >= 9.0f)
                            {
                                frt.Hide();
                            }
                        }
                        else if (percentageNotShowing > frt.GetDisplayThreshold())
                        {
                            frt.Show(level, currentSeconds);
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
                                textManager.Draw(window, gameStatus, gameClock.getElapsedTime().asSeconds());
                                DrawGhosts(window, ghostPersonality);
                                textManager.DrawGhostScore(window, gstPoints, ghostPos);
                                window.display();
                                std::this_thread::sleep_for(std::chrono::milliseconds(1500));
                            }
                            else if (ghostMode != GhostMode::SPAWN)
                            {
                                HideFruits(textManager);
                                RefreshView(window, view);
                                DrawMazeEnvironment(window);
                                DrawPacmanLives(window, lives);
                                textManager.Draw(window, gameStatus, gameClock.getElapsedTime().asSeconds());
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
                                    textManager.Draw(window, gameStatus, gameClock.getElapsedTime().asSeconds());
                                    player.Draw(window);
                                    window.display();

                                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                }

                                lives--;
                                if (lives == 0)
                                {
                                    gameStatus = GameStatus::GAME_OVER;
                                    gameClock.restart();
                                }
                                else
                                {
                                    player.Reset();

                                    for (auto &gst : ghosts)
                                    {
                                        gst.Reset();
                                    }

                                    gameStatus = GameStatus::PRE_GAME;
                                    gameClock.restart();
                                }
                            }
                        }
                    }
                }

                textManager.UpdateScoreText(score);

                // if logic block caused game over, then no need to render for this loop
                if (gameStatus == GameStatus::GAME_OVER)
                {
                    continue;
                }

                // Draw+Display
                RefreshView(window, view);
                DrawMazeEnvironment(window);
                DrawPacmanLives(window, lives);
                textManager.Draw(window, gameStatus, gameClock.getElapsedTime().asSeconds());
                player.Draw(window);
                DrawGhosts(window);

                if (gameStatus == GameStatus::PRE_GAME)
                {
                    textManager.DrawPreGame(window);

                    if (gameClock.getElapsedTime().asSeconds() >= displayDuration)
                    {
                        gameStatus = GameStatus::IN_GAME;
                        player.SetDirection(Direction::LEFT);
                    }
                }
            }
            else
            {
                RefreshView(window, view);
                DrawMazeEnvironment(window);
                DrawPacmanLives(window, lives);
                textManager.Draw(window, gameStatus, gameClock.getElapsedTime().asSeconds());
                textManager.DrawGameOver(window);

                if (gameClock.getElapsedTime().asSeconds() >= displayDuration)
                {
                    gameStatus = GameStatus::MENU;
                }
            }

            // Display new window
            window.display();
        }
    }

    return 0;
}

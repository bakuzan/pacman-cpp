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
#include "include/FileUtils.h"
#include "include/Fruit.h"
#include "include/GameInit.h"
#include "include/GameState.h"
#include "include/GameStatus.h"
#include "include/Ghost.h"
#include "include/Player.h"
#include "include/PickUp.h"
#include "include/RenderUtils.h"
#include "include/SFMLUtils.h"
#include "include/TextManager.h"
#include "include/Wall.h"

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
        WriteToLogFile("Error opening file ./resources/spritesheet.png");
        exit(EXIT_FAILURE);
    }

    // Set colour that will be treated as transparent!
    sf::Color transparentColor = spritesheet.getPixel(0, 0);
    spritesheet.createMaskFromColor(transparentColor);
    SFMLUtils::SetWindowIcon(window, spritesheet);

    sf::Texture sharedTexture;
    sharedTexture.loadFromImage(spritesheet);
    PopulateFruit(sharedTexture);
    PopulatePacmanLives(sharedTexture);

    // Load font for display text
    sf::Font font;
    if (!font.loadFromFile("./resources/PressStart2P-Regular.ttf"))
    {
        WriteToLogFile("Error loading font");
        exit(EXIT_FAILURE);
    }

    // Create managers, controllers, and player
    TextManager textManager(font);
    Player player = Player(sharedTexture, Constants::SPRITE_SIZE);
    GhostModeController *ghostModeController = GhostModeController::GetInstance();

    // Process map
    ReadAndProcessMap(sharedTexture, player);

    // Get min/max x coordinate for the "teleport" tunnel
    auto [minWall, maxWall] = std::minmax_element(GameState::walls.begin(), GameState::walls.end(),
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
        GameState::deltaTime = GameState::deltaClock.restart().asSeconds();
        float fpsLimit = 1.0f / 20.0f; // Lock fps
        if (GameState::deltaTime > fpsLimit)
        {
            GameState::deltaTime = fpsLimit;
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
                    switch (GameState::gameStatus)
                    {
                    case GameStatus::IN_GAME:
                        GameState::gameStatus = GameStatus::PAUSED;
                        break;
                    case GameStatus::PAUSED:
                        GameState::gameStatus = GameStatus::IN_GAME;
                        break;
                    default:
                        break;
                    }
                }
                break;
            }
            }

            if (GameState::gameStatus == GameStatus::MENU)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosView = window.mapPixelToCoords(mousePos);

                newGameButton.HandleEvent(evnt, mousePosView);
                quitButton.HandleEvent(evnt, mousePosView);
            }
        }

        if (GameState::gameStatus == GameStatus::MENU)
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
            if (!(GameState::gameStatus == GameStatus::GAME_OVER))
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
                if (GameState::gameStatus == GameStatus::IN_GAME)
                {
                    // Tick ghost modes
                    ghostModeController->Update(GameState::deltaTime, GameState::ghosts);

                    // Advance player
                    player.Update(newDirection, GameState::deltaTime, GameState::walls, minX, maxX);

                    // Did we eat pellets?
                    sf::Vector2f playerPos = player.GetPosition();
                    int playerX = static_cast<int>(std::round(playerPos.x));
                    int playerY = static_cast<int>(std::round(playerPos.y));
                    for (auto &pu : GameState::pickUps)
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
                                GameState::score += 50;
                            }
                            else
                            {
                                GameState::score += 10;
                            }
                        }
                    }

                    // Are all pellets eaten?
                    bool allPelletsEaten = std::all_of(GameState::pickUps.cbegin(), GameState::pickUps.cend(),
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
                    int totalPickups = GameState::pickUps.size();
                    int countNotShowing = std::count_if(GameState::pickUps.cbegin(), GameState::pickUps.cend(),
                                                        [](const PickUp &pickup)
                                                        { return !pickup.show; });

                    double percentageNotShowing = (static_cast<double>(countNotShowing) / totalPickups) * 100;
                    for (auto &frt : GameState::fruits)
                    {
                        float currentSeconds = GameState::gameClock.getElapsedTime().asSeconds();
                        if (frt.IsShown())
                        {
                            // Has Player eaten the fruit?
                            auto frtPos = frt.GetPosition();
                            if (playerX == frtPos.x &&
                                playerY == frtPos.y)
                            {
                                int fruitPoints = frt.GetPoints();
                                GameState::score += fruitPoints;
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
                            frt.Show(GameState::level, currentSeconds);
                        }
                    }

                    // Advance ghosts
                    sf::FloatRect playerRect(playerPos, sf::Vector2f(Constants::SPRITE_SIZE, Constants::SPRITE_SIZE));
                    for (auto &gst : GameState::ghosts)
                    {
                        gst.Update(GameState::deltaTime, GameState::walls, GameState::ghosts, player, minX, maxX);

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
                                GameState::score += gstPoints;
                                // Pause and show points earn for ghost!
                                RefreshView(window, view);
                                DrawMazeEnvironment(window);
                                DrawFooterGameStatuses(window, GameState::lives);
                                textManager.Draw(window, GameState::gameStatus, GameState::gameClock.getElapsedTime().asSeconds());
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
                                DrawFooterGameStatuses(window, GameState::lives);
                                textManager.Draw(window, GameState::gameStatus, GameState::gameClock.getElapsedTime().asSeconds());
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
                                    DrawFooterGameStatuses(window, GameState::lives);
                                    textManager.Draw(window, GameState::gameStatus, GameState::gameClock.getElapsedTime().asSeconds());
                                    player.Draw(window);
                                    window.display();

                                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                }

                                GameState::lives--;
                                if (GameState::lives == 0)
                                {
                                    GameState::gameStatus = GameStatus::GAME_OVER;
                                    GameState::gameClock.restart();
                                }
                                else
                                {
                                    player.Reset();

                                    for (auto &gst : GameState::ghosts)
                                    {
                                        gst.Reset();
                                    }

                                    GameState::gameStatus = GameStatus::PRE_GAME;
                                    GameState::gameClock.restart();
                                }
                            }
                        }
                    }
                }

                textManager.UpdateScoreText(GameState::score);

                // if logic block caused game over, then no need to render for this loop
                if (GameState::gameStatus == GameStatus::GAME_OVER)
                {
                    continue;
                }

                // Draw+Display
                RefreshView(window, view);
                DrawMazeEnvironment(window);
                DrawFooterGameStatuses(window, GameState::lives);
                textManager.Draw(window, GameState::gameStatus, GameState::gameClock.getElapsedTime().asSeconds());
                player.Draw(window);
                DrawGhosts(window);

                if (GameState::gameStatus == GameStatus::PRE_GAME)
                {
                    textManager.DrawPreGame(window);

                    if (GameState::gameClock.getElapsedTime().asSeconds() >= GameState::displayDuration)
                    {
                        GameState::gameStatus = GameStatus::IN_GAME;
                        player.SetDirection(Direction::LEFT);
                    }
                }
            }
            else
            {
                RefreshView(window, view);
                DrawMazeEnvironment(window);
                DrawFooterGameStatuses(window, GameState::lives);
                textManager.Draw(window, GameState::gameStatus, GameState::gameClock.getElapsedTime().asSeconds());
                textManager.DrawGameOver(window);

                if (GameState::gameClock.getElapsedTime().asSeconds() >= GameState::displayDuration)
                {
                    GameState::gameStatus = GameStatus::MENU;
                }
            }

            // Display new window
            window.display();
        }
    }

    return 0;
}

#include <SFML/Graphics.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/Enums.h"

struct Wall
{
    CellType type;
    float x;
    float y;
};

static const float WINDOW_SIZE = 512.0f;
static const float GRID_WIDTH = 29.0f;
static const float GRID_HEIGHT = 31.0f;

std::vector<Wall> walls;

void writeToLogFile(const std::string &message)
{
    std::ofstream logFile("./logs/log.txt", std::ios_base::app);
    logFile << message << std::endl;
    logFile.close();
}

void ReadAndProcessMap()
{
    std::ifstream file("./resources/map.txt");
    if (!file.is_open())
    {
        writeToLogFile("Error opening file ./resources/map.txt");
        exit(EXIT_FAILURE);
    }

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
            float y = float(lineIndex);

            switch (cellContents)
            {
            case CellType::WALL:
            {
                walls.push_back({CellType::WALL, x, y});
                break;
            }
            case CellType::GHOST_DOOR:
            {
                walls.push_back({CellType::GHOST_DOOR, x, y});
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
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Pacman C++", sf::Style::Close | sf::Style::Resize);
    sf::View view(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(GRID_WIDTH, GRID_HEIGHT));
    view.setCenter(GRID_WIDTH / 2, GRID_HEIGHT / 2);

    ReadAndProcessMap();
    for (auto &wall : walls)
    {
        std::cout << std::to_string(wall.type)
                  << "(" << std::to_string(wall.x) << "," << std::to_string(wall.y) << ")"
                  << std::endl;
    }

    while (window.isOpen())
    {
        // Timing
        // ???

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
                float aspectRatio = float(window.getSize().x) / float(window.getSize().y);
                view.setSize(GRID_HEIGHT * aspectRatio, GRID_HEIGHT);
                break;
            }
            }
        }

        // Logic
        // ???

        // Draw+Display
        window.clear(sf::Color(31, 31, 31));
        window.setView(view);

        for (auto &wall : walls)
        {
            sf::RectangleShape shape;
            float offsetY = 0.0f;

            switch (wall.type)
            {
            case CellType::WALL:
            {
                shape.setSize(sf::Vector2f(0.5f, 0.5f));
                shape.setFillColor(sf::Color::Transparent);
                shape.setOutlineColor(sf::Color(33, 33, 222));
                shape.setOutlineThickness(0.25f);
                shape.setPosition(wall.x, wall.y);
                break;
            }
            case CellType::GHOST_DOOR:
            {
                shape.setSize(sf::Vector2f(1.25f, 0.5f));
                shape.setFillColor(sf::Color(100, 41, 71));
                shape.setPosition(wall.x - 0.25f, wall.y + 0.25f);
                break;
            }
            }

            window.draw(shape);
        }

        // Display new window
        window.display();
    }

    return 0;
}
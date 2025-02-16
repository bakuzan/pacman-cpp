#ifndef RENDERUTILS_H
#define RENDERUTILS_H

#include <SFML/Graphics.hpp>
#include <optional>

#include "GhostPersonality.h"
#include "TextManager.h"

void RefreshView(sf::RenderWindow &window, sf::View &view);
void DrawMazeEnvironment(sf::RenderWindow &window, std::optional<sf::Color> colour = std::nullopt);
void DrawPacmanLives(sf::RenderWindow &window, int lives);
void DrawGhosts(sf::RenderWindow &window, std::optional<GhostPersonality> skipGhostPersonality = std::nullopt);
void FlashWalls(sf::RenderWindow &window, sf::View &view, TextManager &textManager);
void HideFruits(TextManager &textManager);

#endif // RENDERUTILS_H

#ifndef GAMEINIT_H
#define GAMEINIT_H

#include <SFML/Graphics.hpp>
#include "Player.h"
#include "GhostModeController.h"
#include "TextManager.h"

void PopulateFruit(sf::Texture &texture);
void PopulatePacmanLives(sf::Texture &texture);
void InitialiseGame(Player &player, GhostModeController *ghostModeController, bool isNewGame = true);

#endif // GAMEINIT_H

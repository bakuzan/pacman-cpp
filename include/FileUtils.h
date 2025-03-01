#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <SFML/Graphics.hpp>

#include <string>

#include "Player.h"

void WriteToLogFile(const std::string &message);
void ReadAndProcessMap(sf::Texture &sharedTexture, Player &player);
void LoadSoundFiles();
void StopAllSounds();

#endif // FILEUTILS_H

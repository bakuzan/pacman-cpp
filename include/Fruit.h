#ifndef FRUIT_H
#define FRUIT_H

#include <SFML/Graphics.hpp>

class Fruit
{
public:
    Fruit(sf::Texture &sharedTexture, float spriteSize, float displayThreshold);
    ~Fruit();

    float GetDisplayThreshold();
    float GetDisplayTime();
    bool IsShown();
    int GetPoints();
    sf::Vector2f GetPosition();

    void Show(int level, float startTime);
    void Hide();
    void Draw(sf::RenderWindow &window);
    void Reset(int level = 1);

private:
    sf::Texture &texture;
    sf::Sprite sprite;
    float displayThreshold;
    float triggeredTime;
    bool triggered;
    bool show;
    int points;

private:
    void SetTextureRect(int level);
    void SetPoints(int level);
};

#endif // FRUIT_H

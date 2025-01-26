#ifndef FLOATUTILS_H
#define FLOATUTILS_H

#include <SFML/System/Vector2.hpp>

namespace FloatUtils
{
    constexpr float EPSILON = 0.01f;

    bool areFloatsEqual(float a, float b, float epsilon = EPSILON);
    bool arePositionsEqual(const sf::Vector2f &pos1, const sf::Vector2f &pos2, float epsilon = EPSILON);
}

#endif // FLOATUTILS_H

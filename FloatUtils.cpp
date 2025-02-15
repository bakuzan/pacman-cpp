#include "pch.h"

#include <algorithm>
#include <cmath>

#include "include/FloatUtils.h"

namespace FloatUtils
{
    bool areFloatsEqual(float a, float b, float epsilon)
    {
        return std::fabs(a - b) < epsilon;
    }

    bool arePositionsEqual(const sf::Vector2f &pos1, const sf::Vector2f &pos2, float epsilon)
    {
        return areFloatsEqual(pos1.x, pos2.x, epsilon) && areFloatsEqual(pos1.y, pos2.y, epsilon);
    }
}


#include <string>

#include "include/Direction.h"
#include "include/EnumUtils.h"

namespace EnumUtils
{
    std::string DirectionToString(Direction dir)
    {
        switch (dir)
        {
        case NONE:
            return "NONE";
        case UP:
            return "UP";
        case LEFT:
            return "LEFT";
        case DOWN:
            return "DOWN";
        case RIGHT:
            return "RIGHT";
        default:
            return "UNKNOWN";
        }
    }
}
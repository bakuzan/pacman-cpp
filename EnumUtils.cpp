
#include <string>

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

    std::string GhostModeToString(GhostMode mode)
    {
        switch (mode)
        {
        case HOUSED:
            return "HOUSED";
        case LEAVING:
            return "LEAVING";
        case SCATTER:
            return "SCATTER";
        case CHASE:
            return "CHASE";
        case FRIGHTENED:
            return "FRIGHTENED";
        case SPAWN:
            return "SPAWN";
        default:
            return "UNKNOWN";
        }
    }
}
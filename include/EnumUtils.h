#ifndef ENUMUTILS_H
#define ENUMUTILS_H

#include <string>

#include "Direction.h"
#include "GhostMode.h"

namespace EnumUtils
{
    std::string DirectionToString(Direction dir);
    std::string GhostModeToString(GhostMode mode);
}

#endif // ENUMUTILS_H

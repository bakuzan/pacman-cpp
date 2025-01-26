#include "include/Constants.h"
#include "include/Ghost.h"
#include "include/GhostModeController.h"
#include "include/GhostPersonality.h"

#include "include/FloatUtils.h"

GhostModeController::GhostModeController() : mode(GhostMode::SCATTER), timer(0), timeLimit(7)
{
    for (int i = BLINKY; i <= CLYDE; ++i)
    {
        GhostPersonality personality = static_cast<GhostPersonality>(i);
        forceReverseAtNextPossibleMap[personality] = false;
        overrideModeMap[personality] = personality == BLINKY
                                           ? GhostMode::LEAVING
                                           : GhostMode::HOUSED;
    }
}

GhostModeController *GhostModeController::GetInstance()
{
    static GhostModeController instance;
    return &instance;
}

GhostMode GhostModeController::GetMode(GhostPersonality personality)
{
    auto it = overrideModeMap.find(personality);
    if (it != overrideModeMap.end())
    {
        return it->second;
    }

    return mode;
}

void GhostModeController::Update(float deltaTime, const std::vector<Ghost> &ghosts)
{
    // Manage global mode
    timer += deltaTime;
    if (timer >= timeLimit)
    {
        if (mode == GhostMode::SCATTER)
        {
            Chase();
        }
        else
        {
            Scatter();
        }
    }

    // Manage ghost specific states
    for (const auto &entry : overrideModeMap)
    {
        switch (entry.second)
        {
        case GhostMode::HOUSED:
            // TODO Check if can now leave the house; do below if can
            // if (canLeaveHouse)
            // {
            //     overrideModeMap[entry.first] = GhostMode::LEAVING;
            // }
            break;
        case GhostMode::LEAVING:
        {
            auto it = std::find_if(ghosts.cbegin(), ghosts.cend(), [&entry](const Ghost &ghost)
                                   { return ghost.GetPersonality() == entry.first; });

            sf::Vector2f position = it->GetPosition();
            if (FloatUtils::arePositionsEqual(position, Constants::FIRST_OUTSIDE_CELL_LHS, Constants::COLLISION_TOLERANCE))
            {
                overrideModeMap.erase(entry.first);
            }
            break;
        }
        case GhostMode::FRIGHTENED: // TODO
        case GhostMode::SPAWN:      // TODO
        default:
            break;
        }
    }
}

bool GhostModeController::CheckForcedReverseQueue(GhostPersonality personality)
{
    auto mode = GetMode(personality);

    if (mode == GhostMode::SCATTER ||
        mode == GhostMode::CHASE)
    {
        bool forcedReverse = forceReverseAtNextPossibleMap[personality];
        forceReverseAtNextPossibleMap[personality] = false;
        return forcedReverse;
    }

    return false;
}

// Private

void GhostModeController::Chase()
{
    mode = GhostMode::CHASE;
    timeLimit = 20;
    timer = 0;
    QueueForcedReverse();
    std::cout << "CHASE START" << std::endl;
}

void GhostModeController::Scatter()
{
    mode = GhostMode::SCATTER;
    timeLimit = 7;
    timer = 0;
    QueueForcedReverse();
    std::cout << "SCATTER START" << std::endl;
}

void GhostModeController::QueueForcedReverse()
{
    for (int i = BLINKY; i <= CLYDE; ++i)
    {
        GhostPersonality personality = static_cast<GhostPersonality>(i);
        forceReverseAtNextPossibleMap[personality] = true;
    }
}
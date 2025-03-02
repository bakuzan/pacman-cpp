#include "pch.h"

#include "include/Constants.h"
#include "include/GameState.h"
#include "include/Ghost.h"
#include "include/GhostModeController.h"
#include "include/GhostPersonality.h"

#include "include/FloatUtils.h"

GhostModeController::GhostModeController() : mode(GhostMode::SCATTER), timer(0), timeLimit(7), frightenedTimer(0), frightenedTimeLimit(0)
{
    Init();
}

GhostModeController *GhostModeController::GetInstance()
{
    static GhostModeController instance;
    return &instance;
}

GhostMode GhostModeController::GetMode(GhostPersonality personality)
{
    auto fit = frightenedModeMap.find(personality);
    if (fit != frightenedModeMap.end())
    {
        return fit->second;
    }

    return GetModeIgnoringFrightened(personality);
}

GhostMode GhostModeController::GetModeIgnoringFrightened(GhostPersonality personality)
{
    auto oit = overrideModeMap.find(personality);
    if (oit != overrideModeMap.end())
    {
        GhostMode overrideMode = oit->second;
        if (overrideMode == GhostMode::LEAVING)
        {
            // We want to stagger ghosts leaving the house
            float gameSeconds = GameState::gameClock.getElapsedTime().asSeconds();

            if ((personality == GhostPersonality::PINKY &&
                 gameSeconds < (GameState::displayDuration + 1.0f)) ||
                (personality == GhostPersonality::INKY &&
                 gameSeconds < (GameState::displayDuration + 2.0f)) ||
                (personality == GhostPersonality::CLYDE &&
                 gameSeconds < (GameState::displayDuration + 3.0f)))
            {
                overrideMode = GhostMode::HOUSED;
            }
        }

        return overrideMode;
    }

    return mode;
}

void GhostModeController::Update(float deltaTime, float gameTime, double percentagePelletsEaten, const std::vector<Ghost> &ghosts)
{
    // Manage global mode
    if (frightenedTimeLimit == 0)
    {
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
    }
    // Manage frightened mode
    else
    {
        frightenedTimer += deltaTime;
        if (frightenedTimer >= frightenedTimeLimit)
        {
            frightenedTimeLimit = 0;
            frightenedTimer = 0;
            frightenedModeMap.clear();
            std::cout << "FRIGHTENED END" << std::endl;
        }
    }

    // Manage ghost specific states
    for (const auto &entry : overrideModeMap)
    {
        switch (entry.second)
        {
        case GhostMode::HOUSED:
        {
            bool canLeaveHouse = false;
            switch (entry.first)
            {
            case GhostPersonality::PINKY:
                canLeaveHouse = gameTime >= (GameState::displayDuration + 3.5f);
                break;
            case GhostPersonality::INKY:
                canLeaveHouse = percentagePelletsEaten >= 12.5;
                break;
            case GhostPersonality::CLYDE:
                canLeaveHouse = percentagePelletsEaten >= 25.0;
                break;
            default:
                break;
            }

            if (canLeaveHouse)
            {
                overrideModeMap[entry.first] = GhostMode::LEAVING;
            }
            break;
        }
        case GhostMode::LEAVING:
        case GhostMode::SPAWN:
        {
            auto it = std::find_if(ghosts.cbegin(), ghosts.cend(), [&entry](const Ghost &ghost)
                                   { return ghost.GetPersonality() == entry.first; });

            sf::Vector2f position = it->GetPosition();
            if (FloatUtils::arePositionsEqual(position, Constants::FIRST_OUTSIDE_CELL_LHS, Constants::COLLISION_TOLERANCE))
            {
                if (entry.second == GhostMode::SPAWN)
                {
                    overrideModeMap[entry.first] = GhostMode::ENTERING;
                }
                else
                {
                    overrideModeMap.erase(entry.first);
                }
            }
            break;
        }
        case GhostMode::ENTERING:
        {
            auto it = std::find_if(ghosts.cbegin(), ghosts.cend(), [&entry](const Ghost &ghost)
                                   { return ghost.GetPersonality() == entry.first; });

            sf::Vector2f spawnPosition = it->GetSpawnPosition();
            sf::Vector2f position = it->GetPosition();
            if (FloatUtils::arePositionsEqual(position, spawnPosition, Constants::COLLISION_TOLERANCE))
            {
                overrideModeMap[entry.first] = GhostMode::LEAVING;
            }
        }
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

void GhostModeController::StartFrightened()
{
    frightenedTimeLimit = 6;
    frightenedTimer = 0;

    for (int i = BLINKY; i <= CLYDE; ++i)
    {
        GhostPersonality personality = static_cast<GhostPersonality>(i);
        frightenedModeMap[personality] = GhostMode::FRIGHTENED;
    }
    std::cout << "FRIGHTENED START" << std::endl;
}

float GhostModeController::GetFrightenedTimer()
{
    return frightenedTimer;
}

int GhostModeController::GetFrightenedCount()
{
    return frightenedModeMap.size();
}

void GhostModeController::Eaten(GhostPersonality personality)
{
    frightenedModeMap.erase(personality);
    overrideModeMap[personality] = GhostMode::SPAWN;
    std::cout << "Ghost("
              << personality
              << ") Eaten, returning to Spawn"
              << std::endl;
}

void GhostModeController::ResetToHouse(GhostPersonality personality)
{
    auto it = overrideModeMap.find(personality);
    if (it == overrideModeMap.end() || it->second != GhostMode::HOUSED)
    {
        overrideModeMap[personality] = GhostMode::LEAVING;
    }
}

void GhostModeController::Reset()
{
    Init();
}

// Private

void GhostModeController::Init()
{
    mode = GhostMode::SCATTER;
    timer = 0;
    timeLimit = 7;
    frightenedTimer = 0;
    frightenedTimeLimit = 0;
    frightenedModeMap.clear();

    for (int i = BLINKY; i <= CLYDE; ++i)
    {
        GhostPersonality personality = static_cast<GhostPersonality>(i);
        forceReverseAtNextPossibleMap[personality] = false;
        overrideModeMap[personality] = personality == BLINKY
                                           ? GhostMode::LEAVING
                                           : GhostMode::HOUSED;
    }
}

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
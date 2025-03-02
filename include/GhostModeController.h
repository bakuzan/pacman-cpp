#ifndef GHOSTMODECONTROLLER_H
#define GHOSTMODECONTROLLER_H

#include <map>

#include "GhostMode.h"
#include "GhostPersonality.h"

class Ghost;

class GhostModeController
{
public:
    static GhostModeController *GetInstance();

    GhostMode GetMode(GhostPersonality personality);
    GhostMode GetModeIgnoringFrightened(GhostPersonality personality);
    float GetFrightenedTimer();
    int GetFrightenedCount();
    bool CheckForcedReverseQueue(GhostPersonality personality);
    void StartFrightened();
    void Eaten(GhostPersonality personality);
    void Update(float deltaTime, float gameTime, double percentagePelletsEaten, const std::vector<Ghost> &ghosts);
    void ResetToHouse(GhostPersonality personality);
    void Reset();

private:
    std::map<GhostPersonality, GhostMode> overrideModeMap;
    std::map<GhostPersonality, GhostMode> frightenedModeMap;
    std::map<GhostPersonality, bool> forceReverseAtNextPossibleMap;
    GhostMode mode;

    float timer;
    float timeLimit;

    float frightenedTimer;
    float frightenedTimeLimit;

private:
    GhostModeController();
    ~GhostModeController() {}

    GhostModeController(const GhostModeController &) = delete;            // Prevent copy-construction
    GhostModeController &operator=(const GhostModeController &) = delete; // Prevent assignment

    void Init();
    void Chase();
    void Scatter();
    void QueueForcedReverse();
};

#endif // GHOSTMODECONTROLLER_H

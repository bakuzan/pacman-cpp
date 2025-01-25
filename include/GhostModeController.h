#ifndef GHOSTMODECONTROLLER_H
#define GHOSTMODECONTROLLER_H

#include <map>

#include "GhostMode.h"

class Ghost;

class GhostModeController
{
public:
    static GhostModeController *GetInstance();

    GhostMode GetMode(GhostPersonality personality);
    void Update(float deltaTime, const std::vector<Ghost> &ghosts);

private:
    std::map<GhostPersonality, GhostMode> overrideModeMap;
    GhostMode mode;
    float timer;
    float timeLimit;

private:
    GhostModeController();
    ~GhostModeController() {}

    GhostModeController(const GhostModeController &) = delete;            // Prevent copy-construction
    GhostModeController &operator=(const GhostModeController &) = delete; // Prevent assignment

    void Chase();
    void Scatter();
};

#endif // GHOSTMODECONTROLLER_H
